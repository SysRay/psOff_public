#include "common.h"
#include "logging.h"
#include "types.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>
#include <zlib.h>

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
      READY,
    };

    Item(const void* src, const uint32_t srcLen, void* dst, uint32_t dstLen): m_src(src), m_srcLen(srcLen), m_dst(dst), m_dstLen(dstLen) {};
    virtual ~Item() = default;

    inline bool isFinished() { return m_zret == Z_OK && m_status == Status::READY; }

    inline bool isErrored() { return m_zret != Z_OK; }

    inline Status getStatus() const { return m_status; }

    std::mutex& getMutex() { return m_mtx; }

    void startDecompression() {
      if ((m_zret = inflateInit(&m_zstrm)) == Z_OK) {
        m_status = Status::PROCESSING;
        return;
      }

      m_zstrm.next_in  = (Bytef*)m_src;
      m_zstrm.avail_in = m_srcLen;

      m_zstrm.next_out  = (Bytef*)m_dst;
      m_zstrm.avail_out = m_dstFinalLen;

      m_status = Status::ERROR;
    }

    void decompressionStep() {
      if ((m_zret = inflate(&m_zstrm, Z_NO_FLUSH)) != Z_OK) {
        switch (m_zret) {
          case Z_NEED_DICT:
          case Z_DATA_ERROR:
          case Z_MEM_ERROR: {
            m_status = Status::ERROR;
            return;
          }

          // There are some non-fatal errors we can safely ignore
          default: {
            m_zret = Z_OK;
          }
        }
      }

      if (m_zstrm.avail_in == 0) {
        m_status      = Status::DONE;
        m_dstFinalLen = m_zstrm.total_out;
      }
    }

    void finishDecompression() {
      if (!m_bClosed) {
        inflateEnd(&m_zstrm);
        m_bClosed = true;
      }
    }

    void ready() {
      finishDecompression();
      m_status = Status::READY;
    }

    int32_t zlibErrorToSce() {
      // todo convert m_zret to sce code;
      return Err::Zlib::FATAL;
    }

    uint32_t getFinalLength() { return m_status == Status::DONE ? m_dstFinalLen : 0; }

private:
    Status m_status  = Status::IDLE;
    bool   m_bClosed = false;

    const void*    m_src    = nullptr;
    const uint32_t m_srcLen = 0;

    void*    m_dst         = nullptr;
    uint32_t m_dstLen      = 0;
    uint32_t m_dstFinalLen = 0;

    z_stream m_zstrm = {};
    int      m_zret  = 0;

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

        case Item::Status::ERROR:
        case Item::Status::DONE: {
          qi->ready();
        } break;
      }
    }
  }

  void clearQueue() {
    std::unique_lock lock(m_queueMut);

    for (auto it = m_queue.begin(); it != m_queue.end();) {
      it->get()->finishDecompression();
      m_queue.erase(it);
    }
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
  if (src == nullptr || dst == nullptr || reqId == nullptr) return Err::Zlib::INVALID;
  if (srcLen == 0 || dstLen == 0) return Err::Zlib::NOSPACE;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"Inflate request: %p[%u], %p[%u] -> %p", src, srcLen, dst, dstLen, reqId);
  *reqId = accessZlibQueue().addTask(src, srcLen, dst, dstLen);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibWaitForDone(uint64_t* reqId, uint32_t* timeout) {
  if (!accessZlibQueue().inited()) return Err::Zlib::NOT_INITIALIZED;
  if (timeout == nullptr || reqId == nullptr) return Err::Zlib::INVALID;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"Waiting for queue item: %p, %p", reqId, timeout);
  return accessZlibQueue().waitForDone(reqId, timeout) ? Ok : Err::Zlib::TIMEDOUT;
}

EXPORT SYSV_ABI int32_t sceZlibGetResult(uint64_t reqId, uint32_t* dstLen, int* status) {
  if (!accessZlibQueue().inited()) return Err::Zlib::NOT_INITIALIZED;
  if (dstLen == nullptr || status == nullptr) return Err::Zlib::INVALID;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"Get result for: %p, %p, %p", reqId, dstLen, status);
  return accessZlibQueue().getResult(reqId, dstLen, status);
}
}
