#include "common.h"
#include "logging.h"
#include "types.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

LOG_DEFINE_MODULE(libSceZlib);

namespace {
class ZLibQueue {
  class Item {
public:
    enum class Status : uint32_t {
      IDLE,
      PROCESSING,
      DONE,
      ERROR,
    };

    Item(const void* src, const uint32_t srcLen, void* dst, uint32_t dstLen): m_src(src), m_srcLen(srcLen), m_dst(dst), m_dstLen(dstLen) {};
    virtual ~Item() = default;

    inline bool isFinished() { return m_status == Status::DONE || m_status == Status::ERROR; }

    inline bool isErrored() { return m_status == Status::ERROR; }

    inline Status getStatus() const { return m_status; }

    std::mutex& getMutex() { return m_mtx; }

    void startDecompression() { m_status = Status::PROCESSING; }

    void decompressionStep() {
      LOG_USE_MODULE(libSceZlib);
      LOG_ERR(L"Unimplemented");
      m_status = Status::ERROR;
    }

    int32_t zlibErrorToSce() {
      // todo convert m_zret to sce code;
      return Err::Zlib::FATAL;
    }

    uint32_t getFinalLength() { return m_status == Status::DONE ? m_dstFinalLen : 0; }

private:
    Status m_status = Status::IDLE;

    const void*    m_src;
    const uint32_t m_srcLen;

    void*    m_dst;
    uint32_t m_dstLen;
    uint32_t m_dstFinalLen;

    int m_zret;

    std::mutex m_mtx;
  };

  public:
  ZLibQueue() {};
  virtual ~ZLibQueue() = default;

  inline bool inited() const { return m_bInitialized; }

  void step() {
    std::unique_lock lock(m_queueMut);

    for (auto& qi: m_queue) {
      std::unique_lock ilock(qi->getMutex());

      switch (qi->getStatus()) {
        case Item::Status::IDLE: {
          qi->startDecompression();
        } break;

        case Item::Status::PROCESSING: {
          qi->decompressionStep();
        } break;

        case Item::Status::DONE: {
        } break;

        case Item::Status::ERROR: {
        } break;
      }
    }
  }

  void clearQueue() {
    std::unique_lock lock(m_queueMut);
    m_queue.clear();
  }

  static void processingThread(ZLibQueue* zq) {
    while (zq->inited()) {
      zq->step();
    }

    zq->clearQueue();
  }

  void init() {
    m_bInitialized = true;

    std::thread th(processingThread, this);
    th.detach();
  }

  void deinit() { m_bInitialized = false; }

  uint64_t addTask(const void* src, uint32_t srcLen, void* dst, uint32_t dstLen) {
    std::unique_lock lock(m_queueMut);
    m_queue.push_back(std::make_unique<Item>(src, srcLen, dst, dstLen));
    return (uint64_t)&m_queue.back();
  }

  bool waitForDone(uint64_t* reqId, uint32_t* timeout) {
    std::unique_lock lock(m_queueMut);

    auto pred = [reqId, this]() -> bool {
      for (auto& qi: m_queue) {
        if (qi->isFinished()) {
          *reqId = (uint64_t)&qi;
          return true;
        }
      }

      return false;
    };

    if (timeout != nullptr) {
      return m_cond.wait_for(lock, std::chrono::microseconds(*timeout), pred);
    }

    m_cond.wait(lock, pred);
    return true;
  }

  int32_t getResult(uint64_t reqId, uint32_t* outLen, int* status) {
    Item*   zqi = nullptr;
    int32_t ret = Ok;

    auto it = m_queue.begin();
    for (; it != m_queue.end(); ++it) {
      if (it->get() == (Item*)reqId) {
        zqi = it->get();
        break;
      }
    }

    if (zqi == nullptr) return Err::Zlib::NOT_FOUND;
    std::unique_lock lock(zqi->getMutex());
    if (!zqi->isFinished()) return Err::Zlib::AGAIN;

    if (zqi->isErrored()) {
      *outLen = 0;
      *status = -1;
      ret     = zqi->zlibErrorToSce();
    } else {
      *status = 0;
      *outLen = zqi->getFinalLength();
      ret     = Ok;
    }

    m_queue.erase(it);
    return Ok;
  }

  private:
  bool                               m_bInitialized = false;
  std::mutex                         m_queueMut;
  std::condition_variable            m_cond;
  std::vector<std::unique_ptr<Item>> m_queue = {};
};

ZLibQueue& accessZlibQueue() {
  static ZLibQueue zq;
  return zq;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceZlib";

EXPORT SYSV_ABI int32_t sceZlibInitialize(const void* buffer, size_t len) {
  if (accessZlibQueue().inited()) return Err::Zlib::ALREADY_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  accessZlibQueue().init();
  LOG_TRACE(L"Queue initialized");
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibFinalize() {
  if (!accessZlibQueue().inited()) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  accessZlibQueue().deinit();
  LOG_TRACE(L"Queue finalized");
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibInflate(const void* src, uint32_t srcLen, void* dst, uint32_t dstLen, uint64_t* reqId) {
  if (!accessZlibQueue().inited()) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"Inflate request: %p[%u], %p[%u] -> %p", src, srcLen, dst, dstLen, reqId);
  *reqId = accessZlibQueue().addTask(src, srcLen, dst, dstLen);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibWaitForDone(uint64_t* reqId, uint32_t* timeout) {
  if (!accessZlibQueue().inited()) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"Waiting for queue item: %p, %p", reqId, timeout);
  return accessZlibQueue().waitForDone(reqId, timeout) ? Ok : Err::Zlib::TIMEDOUT;
}

EXPORT SYSV_ABI int32_t sceZlibGetResult(uint64_t reqId, uint32_t* dstLen, int* status) {
  if (!accessZlibQueue().inited()) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"Get result for: %p, %p, %p", reqId, dstLen, status);
  return accessZlibQueue().getResult(reqId, dstLen, status);
}
}
