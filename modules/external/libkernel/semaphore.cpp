#include "core/kernel/semaphore.h"

#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/interprocess/sync/interprocess_semaphore.hpp"
#include "common.h"
#include "core/kernel/errors.h"
#include "logging.h"
#include "types.h"

using ScePthreadSem_t = ISemaphore*;
extern "C" {
EXPORT SYSV_ABI int scePthreadSemInit(boost::interprocess::interprocess_semaphore** sem, int pshared, unsigned int initCount, const char* name) {
  *sem = new boost::interprocess::interprocess_semaphore(initCount);
  return Ok;
}

EXPORT SYSV_ABI int scePthreadSemDestroy(ScePthreadSem_t* sem) {
  if (sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  delete *sem;
  *sem = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int scePthreadSemTimedwait(boost::interprocess::interprocess_semaphore** sem, SceKernelUseconds usec) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }

  auto deadline = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::microseconds(usec);
  return (*sem)->timed_wait(deadline) ? Ok : POSIX_SET(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int scePthreadSemPost(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  (*sem)->post();
  return Ok;
}

EXPORT SYSV_ABI int scePthreadSemTrywait(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }

  return (*sem)->try_wait() ? Ok : POSIX_SET(ErrCode::_EBUSY);
}

EXPORT SYSV_ABI int scePthreadSemWait(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }

  (*sem)->wait();
  return Ok;
}
}

EXPORT SYSV_ABI int sceKernelCreateSema(ScePthreadSem_t* sem, const char* name, uint32_t attr, int init, int max, const void* opt) {
  if (sem == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*sem) = createSemaphore_fifo(name, init, max).release();
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteSema(ScePthreadSem_t sem) {
  if (sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }

  delete sem;
  return Ok;
}

EXPORT SYSV_ABI int sceKernelWaitSema(ScePthreadSem_t sem, int need, SceKernelUseconds* time) {
  if (sem == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  return sem->wait(need, time);
}

EXPORT SYSV_ABI int sceKernelPollSema(ScePthreadSem_t sem, int need) {
  if (sem == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  return sem->poll(need);
}

EXPORT SYSV_ABI int sceKernelSignalSema(ScePthreadSem_t sem, int count) {
  if (sem == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  return sem->signal(count);
}

EXPORT SYSV_ABI int sceKernelCancelSema(ScePthreadSem_t sem, int count, int* threads) {
  if (sem == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  return sem->cancel(count, threads);
}