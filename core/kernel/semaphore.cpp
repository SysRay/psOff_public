#include "semaphore.h"

#include "logging.h"
#include "modules_include/common.h"

#include <queue>

#include <boost/chrono.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

LOG_DEFINE_MODULE(Semaphore)

namespace {
size_t getUniqueId() {
  static size_t count = 0;
  return count++;
}
} // namespace

class Semaphore: public ISemaphore {

  enum class Status { Set, Canceled, Deleted };

  boost::mutex              m_mutex;
  boost::condition_variable m_condState;

  std::queue<std::shared_ptr<boost::condition_variable>> m_condQueue; // todo: optimze

  std::string  m_name;
  size_t       m_waitCounter  = 0;
  size_t const m_id           = getUniqueId();
  int          m_countThreads = 0;

  Status m_state = Status::Set;
  bool   m_fifo;
  int    m_count;
  int    m_max_count;

  public:
  Semaphore(const std::string& name, bool fifo, int init_count, int max_count): m_name(name), m_fifo(fifo), m_count(init_count), m_max_count(max_count) {
    if (!fifo) m_condQueue.push(std::make_shared<boost::condition_variable>());
  };

  virtual ~Semaphore();

  int cancel(int setCount, int* numWaitingThreads) final;
  int signal(int signalCount) final;
  int wait(int needcount, uint32_t* pMicros) final;
  int try_wait(int needcount, uint32_t* pMicros) final;

  std::string_view const getName() const final { return m_name; }

  size_t getId() const final { return m_id; }

  private:
  int wait_internal(int needCount, uint32_t* pMicros, boost::unique_lock<boost::mutex>& lock);
};

std::unique_ptr<ISemaphore> createSemaphore(const char* name, bool fifo, int initCount, int maxCount) {
  return std::make_unique<Semaphore>(name == nullptr ? "" : name, fifo, initCount, maxCount);
}

Semaphore::~Semaphore() {
  boost::unique_lock lock(m_mutex);
  m_state = Status::Deleted;

  while (!m_condQueue.empty()) {
    m_condQueue.front()->notify_all();
    m_condQueue.pop();
  }

  // Wait for Threads to leave wait
  m_condState.wait(lock, [this] { return m_countThreads == 0; });
}

int Semaphore::cancel(int setCount, int* numWaitingThreads) {
  boost::unique_lock lock(m_mutex);

  m_state = Status::Canceled;
  if (numWaitingThreads != nullptr) *numWaitingThreads = m_countThreads;

  if (m_fifo) {
    while (!m_condQueue.empty()) {
      m_condQueue.front()->notify_one();
      m_condQueue.pop();
    }
  } else {
    m_condQueue.front()->notify_all();
  }

  // Wait for Threads to leave wait
  m_condState.wait(lock, [this] { return m_countThreads == 0; });
  m_state = Status::Set;
  return Ok;
}

int Semaphore::signal(int signalCount) {
  LOG_USE_MODULE(Semaphore);

  boost::unique_lock lock(m_mutex);
  LOG_TRACE(L"KernelSema(%llu) name:%S signal:%d count:%d", m_id, m_name.c_str(), signalCount, m_count);

  m_condState.wait(lock, [this] { return m_state != Status::Canceled; });
  if (m_state == Status::Deleted) {
    return getErr(ErrCode::_EACCES);
  }

  if (m_count + signalCount > m_max_count) {
    return getErr(ErrCode::_EINVAL);
  }
  if (signalCount > 0) {
    m_count += signalCount;
    if (!m_condQueue.empty()) m_condQueue.front()->notify_one();
  }
  return Ok;
}

int Semaphore::wait_internal(int needCount, uint32_t* pMicros, boost::unique_lock<boost::mutex>& lock) {
  LOG_USE_MODULE(Semaphore);
  std::chrono::time_point<std::chrono::system_clock> start     = std::chrono::system_clock::now();
  uint32_t const                                     micros    = pMicros != nullptr ? *pMicros : 0;
  size_t                                             waitCount = 0;
  {

    if (m_state == Status::Deleted) return getErr(ErrCode::_EACCES);

    std::shared_ptr<boost::condition_variable> condVar;
    if (m_fifo) {
      condVar = std::make_shared<boost::condition_variable>();
      m_condQueue.push(condVar);
    } else {
      condVar = m_condQueue.front();
    }

    waitCount = m_waitCounter++;
    LOG_TRACE(L"-> KernelSema(%llu) name:%S waitCount:%llu need:%d count:%d time:%u us", m_id, m_name.c_str(), waitCount, needCount, m_count, micros);

    m_countThreads++;
    if (pMicros != nullptr) {
      if (!condVar->wait_for(lock, boost::chrono::microseconds(micros),
                             [this, condVar, needCount] { return m_state != Status::Set || (m_condQueue.front() == condVar && m_count >= needCount); })) {
        LOG_WARN(L"<- KernelSema(%llu) name:%S waitCount:%llu timeout", m_id, m_name.c_str(), waitCount);
        *pMicros = 0;
        m_countThreads--;
        return getErr(ErrCode::_ETIMEDOUT);
      }
    } else {
      condVar->wait(lock, [this, condVar, needCount] { return m_state != Status::Set || (m_condQueue.front() == condVar && m_count >= needCount); });
    }

    if (m_fifo) m_condQueue.pop();
    m_count -= needCount;
    m_countThreads--;

    m_condState.notify_all();

    if (m_count >= 0 && !m_condQueue.empty()) m_condQueue.front()->notify_one(); // notify next in queue if count is != 0
  }
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();
  LOG_TRACE(L"<- KernelSema(%llu) name:%S waitCount:%llu count:%d waiting:%llu", m_id, m_name.c_str(), waitCount, m_count, m_condQueue.size());

  if (pMicros != nullptr) {
    *pMicros = (elapsed >= micros ? 0 : micros - elapsed);
  }

  if (m_state == Status::Canceled) {
    return getErr(ErrCode::_ECANCELED);
  }

  if (m_state == Status::Deleted) {
    return getErr(ErrCode::_EACCES);
  }

  return Ok;
}

int Semaphore::wait(int needCount, uint32_t* pMicros) {

  boost::unique_lock lock(m_mutex);
  return wait_internal(needCount, pMicros, lock);
}

int Semaphore::try_wait(int needCount, uint32_t* pMicros) {
  boost::unique_lock lock(m_mutex);
  if (m_countThreads > 0) return getErr(ErrCode::_EAGAIN);

  return wait_internal(needCount, pMicros, lock);
}