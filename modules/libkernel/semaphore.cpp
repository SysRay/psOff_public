#include "core/kernel/semaphore.h"

#include "common.h"
#include "core/kernel/errors.h"
#include "logging.h"
#include "types.h"

using ScePthreadSem_t = ISemaphore*;
extern "C" {
EXPORT SYSV_ABI int scePthreadSemInit(ScePthreadSem_t* sem, int pshared, unsigned int initCount, const char* name) {
  (*sem) = createSemaphore(name, false, 0, std::numeric_limits<int>::max()).release();
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

EXPORT SYSV_ABI int scePthreadSemTimedwait(ScePthreadSem_t* sem, SceKernelUseconds usec) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }

  return POSIX_CALL((*sem)->wait(1, &usec));
}

EXPORT SYSV_ABI int scePthreadSemPost(ScePthreadSem_t* sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return POSIX_CALL((*sem)->signal(1));
}

EXPORT SYSV_ABI int scePthreadSemTrywait(ScePthreadSem_t* sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return POSIX_CALL((*sem)->try_wait(1, nullptr));
}

EXPORT SYSV_ABI int scePthreadSemWait(ScePthreadSem_t* sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return POSIX_CALL((*sem)->wait(1, nullptr));
}
}

EXPORT SYSV_ABI int sceKernelCreateSema(ScePthreadSem_t* sem, const char* name, uint32_t attr, int init, int max, const void* opt) {
  if (name == nullptr || init < 0 || init > max) {
    return getErr(ErrCode::_EINVAL);
  }

  bool fifo = attr == 1 ? true : false;

  (*sem) = createSemaphore(name, fifo, init, max).release();
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