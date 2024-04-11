#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/interprocess/sync/interprocess_semaphore.hpp"
#include "common.h"
#include "core/kernel/errors.h"
#include "core/kernel/semaphore.h"
#include "core/timer/timer.h"
#include "logging.h"
#include "types.h"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>

LOG_DEFINE_MODULE(libScePosix);

using ScePthreadSem_t = ISemaphore*;

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

EXPORT SYSV_ABI int __NID(sem_init)(boost::interprocess::interprocess_semaphore** sem, int pshared, unsigned int value) {
  *sem = new boost::interprocess::interprocess_semaphore(value);
  return Ok;
}

EXPORT SYSV_ABI int __NID(sem_destroy)(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  delete *sem;
  *sem = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int __NID(sem_post)(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  (*sem)->post();
  return Ok;
}

// EXPORT SYSV_ABI int sem_reltimedwait_np(ScePthreadSem_t*sem, useconds_t);
EXPORT SYSV_ABI int __NID(sem_trywait)(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  return (*sem)->try_wait() ? Ok : POSIX_SET(ErrCode::_EBUSY);
}

// EXPORT SYSV_ABI int sem_unlink(const char* semName){}
EXPORT SYSV_ABI int __NID(sem_wait)(boost::interprocess::interprocess_semaphore** sem) {
  if (sem == nullptr || *sem == nullptr) {
    return POSIX_SET(ErrCode::_ESRCH);
  }
  (*sem)->wait();
  return Ok;
}

EXPORT SYSV_ABI unsigned int __NID(sleep)(unsigned int seconds) {
  boost::this_thread::sleep_for(boost::chrono::seconds(seconds));
  return Ok;
}

EXPORT SYSV_ABI int __NID(usleep)(SceKernelUseconds microseconds) {
  boost::this_thread::sleep_for(boost::chrono::microseconds(microseconds));
  return Ok;
}

EXPORT SYSV_ABI int __NID(nanosleep)(const SceKernelTimespec* rqtp, SceKernelTimespec* rmtp) {
  auto startTime = boost::chrono::high_resolution_clock::now();

  boost::this_thread::sleep_for(boost::chrono::seconds(rqtp->tv_sec) + boost::chrono::nanoseconds(rqtp->tv_nsec));

  if (rmtp != nullptr) {
    auto const endTime = boost::chrono::high_resolution_clock::now();
    auto const diff    = boost::chrono::duration_cast<boost::chrono::nanoseconds>(endTime - startTime).count();
    ns2timespec(rmtp, diff);
  }
  return Ok;
}

EXPORT SYSV_ABI int __NID(clock_getres)(SceKernelClockid clockId, SceKernelTimespec* tp) {
  return POSIX_CALL(accessTimer().getTimeRes(clockId, tp));
}

EXPORT SYSV_ABI int __NID(clock_gettime)(SceKernelClockid clockId, SceKernelTimespec* tp) {
  return POSIX_CALL(accessTimer().getTime(clockId, tp));
}

EXPORT SYSV_ABI int __NID(gettimeofday)(SceKernelTimeval* tp) {
  return POSIX_CALL(accessTimer().getTimeofDay(tp));
}

EXPORT SYSV_ABI int __NID(sched_get_priority_max)(int) {
  return SCE_KERNEL_PRIO_FIFO_HIGHEST;
}

EXPORT SYSV_ABI int __NID(sched_get_priority_min)(int) {
  return SCE_KERNEL_PRIO_FIFO_LOWEST;
}

EXPORT SYSV_ABI int __NID(sched_setparam)(int64_t, const struct sched_param*) {
  LOG_USE_MODULE(libScePosix);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int __NID(sched_getparam)(int64_t, struct sched_param*) {
  LOG_USE_MODULE(libScePosix);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int __NID(sched_yield)(void) {
  boost::this_thread::yield();
  return Ok;
}
}