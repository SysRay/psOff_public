#include "logging.h"
#include "modules_include/common.h"
#include "pthread.h"
#include "semaphore.h"

#include <queue>

#include <boost/chrono.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <map>

LOG_DEFINE_MODULE(Semaphore)

namespace {
enum class SemState { idle, signaled, waiting, canceled };

struct SemData {
  size_t index = 0;

  uint32_t needs = 0;

  SemState state = SemState::idle;

  boost::condition_variable m_condVar;

  SemData* parent = nullptr;
  SemData* child  = nullptr;
};
} // namespace

class Semaphore: public ISemaphore {
  boost::mutex m_mutexInt;

  boost::condition_variable m_condState;

  size_t const m_id = [] {
    static size_t count = 0;
    return count++;
  }();

  std::string const m_name;
  int const         m_maxCount;

  uint32_t m_signalValue = 0;

  bool m_isStop = false;

  std::map<size_t, std::shared_ptr<SemData>> m_semData; // todo use pthread specific

  SemData* m_curBack  = nullptr;
  SemData* m_curStart = nullptr;

  size_t m_numWaiters = 0;

  size_t m_countWaits = 0;

  public:
  Semaphore(const std::string& name, int initCount, int maxCount): m_name(name), m_maxCount(maxCount) { m_signalValue = initCount; };

  virtual ~Semaphore() {
    m_isStop = true;

    boost::unique_lock lock(m_mutexInt);

    while (m_curStart != nullptr) {
      m_curStart->m_condVar.notify_one();
      m_curStart = m_curStart->child;
    }

    m_condState.wait(lock, [this] { return m_numWaiters == 0; });
  }

  // ### Interface
  std::string_view const getName() const final { return m_name; }

  size_t getSignalCounter() const final { return m_signalValue; }

  size_t getId() const final { return m_id; }

  int cancel(int setCount, int* numCanceled) final;
  int signal(int signalCount) final;
  int wait(int needcount, uint32_t* pMicros) final;
  int try_wait(int needcount, uint32_t* pMicros) final;
  int poll(int needCount) final;

  private:
  int wait_internal(int needCount, uint32_t* pMicros, boost::unique_lock<boost::mutex>& lock);

  int poll_internal(int needCount, boost::unique_lock<boost::mutex>& lock);
};

std::unique_ptr<ISemaphore> createSemaphore_fifo(const char* name, int initCount, int maxCount) {
  return std::make_unique<Semaphore>(name == nullptr ? "" : name, initCount, maxCount);
}

int Semaphore::cancel(int setCount, int* numCanceled) {
  LOG_USE_MODULE(Semaphore);

  if (setCount <= 0 || setCount >= m_maxCount) return getErr(ErrCode::_EINVAL);

  boost::unique_lock lock(m_mutexInt);

  *numCanceled = 0;
  auto start   = m_curStart;
  while (start != nullptr) {
    if (start->needs > setCount) break;

    ++*numCanceled;
    setCount -= start->needs;
    start->state = SemState::canceled;

    start = start->child; // next
  }

  m_signalValue = 0;

  // Notify back
  if (*numCanceled > 0) {
    lock.unlock();
    start->m_condVar.notify_one();
  }
  // -
  return Ok;
}

int Semaphore::signal(int signalCount) {
  LOG_USE_MODULE(Semaphore);

  boost::unique_lock lock(m_mutexInt);

  m_signalValue += signalCount;

  // Notify back
  if (m_curStart != nullptr && m_curStart->needs <= m_signalValue) {
    m_curStart->state = SemState::signaled;
    LOG_TRACE(L"KernelSema(%llu) name:%S notify| count:%d index:%llu", m_id, m_name.c_str(), m_signalValue, m_curStart->index);

    auto& cond = m_curStart->m_condVar;
    lock.unlock();
    cond.notify_one(); // race condition if m_curStart is used
  } else {
    LOG_TRACE(L"KernelSema(%llu) name:%S signal| count:%d", m_id, m_name.c_str(), m_signalValue);
  }
  // -
  return Ok;
}

int Semaphore::wait_internal(int needCount, uint32_t* pMicros, boost::unique_lock<boost::mutex>& lock) {
  LOG_USE_MODULE(Semaphore);

  if (poll_internal(needCount, lock) == Ok) {
    return Ok;
  }

  auto itThread = m_semData.find(pthread::getThreadId());
  if (itThread == m_semData.end()) {
    itThread = m_semData.emplace(std::make_pair(pthread::getThreadId(), std::make_shared<SemData>())).first;
  }

  ++m_numWaiters;

  auto ownData   = itThread->second;
  ownData->index = ++m_countWaits;
  ownData->needs = needCount;

  // enque in list
  if (m_curBack != nullptr) {
    m_curBack->child = ownData.get();
  }
  if (m_curStart == nullptr) {
    m_curStart = ownData.get();
  }
  ownData->parent = m_curBack;
  m_curBack       = ownData.get();
  // - list

  int ret        = Ok;
  ownData->state = SemState::waiting;

  LOG_TRACE(L"-> KernelSema(%llu) name:%S  wait| count:%d needs:%d index:%llu state:%d", m_id, m_name.c_str(), m_signalValue, needCount, ownData->index,
            ownData->state);

  if (pMicros == nullptr) {
    ownData->m_condVar.wait(lock, [this, ownData] { return m_isStop || ownData->state != SemState::waiting; });
  } else {
    std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

    if (!ownData->m_condVar.wait_for(lock, boost::chrono::microseconds(*pMicros),
                                     [this, ownData] { return m_isStop || ownData->state != SemState::waiting; })) {
      // timeout
      ret = getErr(ErrCode::_ETIMEDOUT);

      // remove from list
      if (ownData->parent != nullptr) {
        ownData->parent->child = ownData->child;
      }
      if (ownData->child != nullptr) {
        ownData->child->parent = ownData->parent;
      }

      LOG_TRACE(L"<- KernelSema(%llu) name:%S  timeout| count:%d needs:%d index:%llu", m_id, m_name.c_str(), m_signalValue, needCount, ownData->index);

      // Special: first in list -> notify next
      if (ownData->parent == nullptr) {
        m_curStart = ownData->child;
        if (m_curStart != nullptr && (m_isStop || m_curStart->needs <= m_signalValue)) {
          m_curStart->state = SemState::signaled;
          LOG_TRACE(L"KernelSema(%llu) name:%S timeout notify| count:%d index:%llu", m_id, m_name.c_str(), m_signalValue, m_curStart->index);

          auto& cond = m_curStart->m_condVar;
          lock.unlock();
          cond.notify_one(); // race condition if m_curStart is used
        }
      }
      // - special

      // Reset ownData
      ownData->child  = nullptr;
      ownData->parent = nullptr;
      ownData->state  = SemState::idle;

      *pMicros = 0;

      if (--m_numWaiters == 0 && m_isStop) {
        lock.unlock();
        m_condState.notify_one();
      }
      return ret;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count();

    if (pMicros != nullptr) {
      *pMicros = (elapsed >= *pMicros ? 0 : *pMicros - elapsed);
    }
  }

  if (m_isStop || ownData->state == SemState::canceled) {
    ret = getErr(ErrCode::_ECANCELED);
  }

  if (ret == Ok) {
    m_signalValue -= ownData->needs;
  }

  // Set list start/end
  if (ownData->child == nullptr) {
    // Reached end -> reset back
    m_curBack    = nullptr;
    m_countWaits = 0;
  }
  m_curStart = ownData->child;
  // -
  LOG_TRACE(L"<- KernelSema(%llu) name:%S  wait| count:%d needs:%d index:%llu state:%d", m_id, m_name.c_str(), m_signalValue, needCount, ownData->index,
            ownData->state);

  // Reset ownData
  ownData->child  = nullptr;
  ownData->parent = nullptr;
  ownData->state  = SemState::idle;
  // -

  // notify next
  if (m_curStart != nullptr && (m_isStop || m_curStart->needs <= m_signalValue)) {
    m_curStart->state = SemState::signaled;
    LOG_TRACE(L"KernelSema(%llu) name:%S notify| count:%d index:%llu", m_id, m_name.c_str(), m_signalValue, m_curStart->index);

    auto& cond = m_curStart->m_condVar;
    lock.unlock();
    cond.notify_one(); // race condition if m_curStart is used
  }
  //

  if (--m_numWaiters == 0 && m_isStop) {
    lock.unlock();
    m_condState.notify_one();
  }
  return ret;
}

int Semaphore::wait(int needcount, uint32_t* pMicros) {
  boost::unique_lock lock(m_mutexInt);
  return wait_internal(needcount, pMicros, lock);
}

int Semaphore::try_wait(int needcount, uint32_t* pMicros) {
  boost::unique_lock lock(m_mutexInt);

  if (m_curBack != nullptr) {
    return getErr(ErrCode::_EBUSY);
  }

  return wait_internal(needcount, pMicros, lock);
}

int Semaphore::poll_internal(int needCount, boost::unique_lock<boost::mutex>& lock) {
  if (m_curBack == 0 && (needCount <= m_signalValue)) {
    m_signalValue -= needCount;
    return Ok;
  }

  return getErr(ErrCode::_EAGAIN); // Waiters in queue, todo need enqueue?
}

int Semaphore::poll(int needCount) {
  boost::unique_lock lock(m_mutexInt);
  return poll_internal(needCount, lock);
}