#define __APICALL_EXTERN
#include "eventflag.h"
#undef __APICALL_EXTERN

#include "logging.h"
#include "modules_include/common.h"

#include <boost/thread/condition.hpp>
#include <memory>

LOG_DEFINE_MODULE(KernelEventFlag);

namespace Kernel::EventFlag {

class KernelEventFlag: public IKernelEventFlag {
  CLASS_NO_COPY(KernelEventFlag);

  public:
  KernelEventFlag(std::string const& name, bool single, bool /*fifo*/, uint64_t bits): m_singleThread(single), m_bits(bits), m_name(name) {};
  virtual ~KernelEventFlag();

  void set(uint64_t bits) final;
  void clear(uint64_t bits) final;
  void cancel(uint64_t bits, int* num_waiting_threads) final;

  int wait(uint64_t bits, WaitMode wait_mode, ClearMode clear_mode, uint64_t* result, uint32_t* ptr_micros) final;

  int poll(uint64_t bits, WaitMode wait_mode, ClearMode clear_mode, uint64_t* result) final {
    uint32_t micros = 0;
    return wait(bits, wait_mode, clear_mode, result, &micros);
  }

  private:
  enum class Status { Set, Canceled, Deleted };

  void setStatus(Status status) {
    m_status = status;
    if (status == Status::Set) m_cond_var_status.notify_all();
  }

  boost::mutex              m_mutex_cond;
  boost::condition_variable m_cond_var;
  boost::condition_variable m_cond_var_status;

  Status m_status = Status::Set;

  int  m_waitingThreads = 0;
  bool m_singleThread   = false;

  std::string m_name;

  uint64_t m_bits = 0;
};

KernelEventFlag::~KernelEventFlag() {
  boost::unique_lock lock(m_mutex_cond);

  m_cond_var_status.wait(lock, [this] { return m_status == Status::Set; });

  m_status = Status::Deleted;
  m_cond_var.notify_all();

  m_cond_var_status.wait(lock, [this] { return m_waitingThreads == 0; });
}

void KernelEventFlag::set(uint64_t bits) {
  boost::unique_lock lock(m_mutex_cond);

  m_cond_var_status.wait(lock, [this] { return m_status == Status::Set; });
  m_bits |= bits;
  m_cond_var.notify_all();
}

void KernelEventFlag::clear(uint64_t bits) {
  boost::unique_lock lock(m_mutex_cond);

  m_cond_var_status.wait(lock, [this] { return m_status == Status::Set; });
  m_bits &= bits;
}

void KernelEventFlag::cancel(uint64_t bits, int* num_waiting_threads) {
  boost::unique_lock lock(m_mutex_cond);

  m_cond_var_status.wait(lock, [this] { return m_status == Status::Set; });
  if (num_waiting_threads != nullptr) {
    *num_waiting_threads = m_waitingThreads;
  }

  setStatus(Status::Canceled);
  m_bits = bits;

  m_cond_var.notify_all();
  m_cond_var_status.wait(lock, [this] { return m_waitingThreads == 0; });
  setStatus(Status::Set);
}

int KernelEventFlag::wait(uint64_t bits, WaitMode wait_mode, ClearMode clear_mode, uint64_t* result, uint32_t* ptr_micros) {
  LOG_USE_MODULE(KernelEventFlag);
  boost::unique_lock lock(m_mutex_cond);

  if (m_singleThread && m_waitingThreads > 0) {
    return getErr(ErrCode::_EPERM);
  }

  uint32_t micros     = 0;
  bool     infinitely = true;
  if (ptr_micros != nullptr) {
    micros     = *ptr_micros;
    infinitely = false;
  }

  auto const start = boost::chrono::system_clock::now();
  ++m_waitingThreads;
  auto waitFunc = [this, wait_mode, bits] {
    return (m_status == Status::Canceled || m_status == Status::Deleted || (wait_mode == WaitMode::And && (m_bits & bits) == bits) ||
            (wait_mode == WaitMode::Or && (m_bits & bits) != 0));
  };
  if (infinitely) {
    m_cond_var.wait(lock, waitFunc);
  } else {
    if (!m_cond_var.wait_for(lock, boost::chrono::microseconds(micros), waitFunc)) {
      if (result != nullptr) {
        *result = m_bits;
      }
      *ptr_micros = 0;
      --m_waitingThreads;
      return getErr(ErrCode::_ETIMEDOUT);
    }
  }
  --m_waitingThreads;
  m_cond_var_status.notify_all();
  if (result != nullptr) {
    *result = m_bits;
  }

  auto elapsed = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::system_clock::now() - start).count();
  if (result != nullptr) {
    *result = m_bits;
  }

  if (ptr_micros != nullptr) {
    *ptr_micros = (elapsed >= micros ? 0 : micros - elapsed);
    // LOG_TRACE(L"wait %llu us -> %llu us", micros, elapsed);
  }

  // Aborted
  if (m_status == Status::Canceled) {
    return getErr(ErrCode::_ECANCELED);
  } else if (m_status == Status::Deleted) {
    return getErr(ErrCode::_EACCES);
  }
  // -

  if (clear_mode == ClearMode::All) {
    m_bits = 0;
  } else if (clear_mode == ClearMode::Bits) {
    m_bits &= ~bits;
  }

  return Ok;
}

IKernelEventFlag_t createEventFlag(std::string const& name, bool single, bool fifo, uint64_t bits) {
  return std::make_unique<KernelEventFlag>(std::string(name), single, fifo, bits).release();
}

int deleteEventFlag(IKernelEventFlag_t ef) {
  LOG_USE_MODULE(KernelEventFlag);
  if (ef == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }
  LOG_INFO(L"Deleted ptr:0x%08llx", (uint64_t)ef);
  delete ef;
  return Ok;
}
} // namespace Kernel::EventFlag
