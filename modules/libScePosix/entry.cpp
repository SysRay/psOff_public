#include "common.h"
#include "core/kernel/errors.h"
#include "core/semaphore/semaphore.h"
#include "logging.h"
#include "types.h"
LOG_DEFINE_MODULE(libScePosix);

using ScePthreadSem_t = ISemaphore*;

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

EXPORT SYSV_ABI int __NID(sem_init)(ScePthreadSem_t* sem, int pshared, unsigned int value) {
  (*sem) = createSemaphore(nullptr, false, 0, std::numeric_limits<int>::max()).release();
  return Ok;
}

EXPORT SYSV_ABI int __NID(sem_destroy)(ScePthreadSem_t* sem) {
  if (sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  delete *sem;
  *sem = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int __NID(sem_post)(ScePthreadSem_t* sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return POSIX_CALL((*sem)->signal(1));
}

// EXPORT SYSV_ABI int sem_reltimedwait_np(ScePthreadSem_t*sem, useconds_t);
EXPORT SYSV_ABI int __NID(sem_trywait)(ScePthreadSem_t* sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return POSIX_CALL((*sem)->try_wait(1, nullptr));
}

// EXPORT SYSV_ABI int sem_unlink(const char* semName){}
EXPORT SYSV_ABI int __NID(sem_wait)(ScePthreadSem_t* sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return POSIX_CALL((*sem)->wait(1, nullptr));
}
}