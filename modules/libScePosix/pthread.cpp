#include "core/kernel/pthread.h"

#include "common.h"
#include "core/kernel/errors.h"
#include "core/memory/memory.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(pthread);

extern "C" {
EXPORT SYSV_ABI int __NID(pthread_attr_destroy)(ScePthreadAttr* attr) {
  return POSIX_CALL(pthread::attrDestroy(attr));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getstack)(const ScePthreadAttr* attr, void** stackAddr, size_t* stackSize) {
  return POSIX_CALL(pthread::attrGetstack(attr, stackAddr, stackSize));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getstacksize)(const ScePthreadAttr* attr, size_t* stackSize) {
  return POSIX_CALL(pthread::attrGetstacksize(attr, stackSize));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getguardsize)(const ScePthreadAttr* attr, size_t* guardSize) {
  return POSIX_CALL(pthread::attrGetguardsize(attr, guardSize));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getstackaddr)(const ScePthreadAttr* attr, void** stackAddr) {
  return POSIX_CALL(pthread::attrGetstackaddr(attr, stackAddr));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getdetachstate)(const ScePthreadAttr* attr, SceDetachState* state) {
  return POSIX_CALL(pthread::attrGetdetachstate(attr, state));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setstacksize)(ScePthreadAttr* attr, size_t stackSize) {
  return POSIX_CALL(pthread::attrSetstacksize(attr, stackSize));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setguardsize)(ScePthreadAttr* attr, size_t guardSize) {
  return POSIX_CALL(pthread::attrSetguardsize(attr, guardSize));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setstack)(ScePthreadAttr* attr, void* addr, size_t size) {
  return POSIX_CALL(pthread::attrSetstack(attr, addr, size));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setstackaddr)(ScePthreadAttr* attr, void* addr) {
  return POSIX_CALL(pthread::attrSetstackaddr(attr, addr));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setdetachstate)(ScePthreadAttr* attr, SceDetachState state) {
  return POSIX_CALL(pthread::attrSetdetachstate(attr, state));
}

EXPORT SYSV_ABI int __NID(pthread_barrier_destroy)(ScePthreadBarrier* barrier) {
  return POSIX_CALL(pthread::barrierDestroy(barrier));
}

EXPORT SYSV_ABI int __NID(pthread_barrier_init)(ScePthreadBarrier* barrier, const ScePthreadBarrierattr* attr, unsigned count) {
  return POSIX_CALL(pthread::barrierInit(barrier, attr, count, nullptr));
}

EXPORT SYSV_ABI int __NID(pthread_barrier_wait)(ScePthreadBarrier* barrier) {
  return POSIX_CALL(pthread::barrierWait(barrier));
}

EXPORT SYSV_ABI int __NID(pthread_barrierattr_destroy)(ScePthreadBarrierattr* attr) {
  return POSIX_CALL(pthread::barrierattrDestroy(attr));
}

EXPORT SYSV_ABI int __NID(pthread_barrierattr_init)(ScePthreadBarrierattr* attr) {
  return POSIX_CALL(pthread::barrierattrInit(attr));
}

EXPORT SYSV_ABI int __NID(pthread_barrierattr_getpshared)(ScePthreadBarrierattr* barrier, int* pshared) {
  return POSIX_CALL(pthread::barrierattrGetpshared(barrier, pshared));
}

EXPORT SYSV_ABI int __NID(pthread_barrierattr_setpshared)(ScePthreadBarrierattr* barrier, int pshared) {
  return POSIX_CALL(pthread::barrierattrSetpshared(barrier, pshared));
}

EXPORT SYSV_ABI int __NID(pthread_condattr_destroy)(ScePthreadCondattr* attr) {
  return POSIX_CALL(pthread::condattrDestroy(attr));
}

EXPORT SYSV_ABI int __NID(pthread_condattr_init)(ScePthreadCondattr* attr) {
  return POSIX_CALL(pthread::condattrInit(attr));
}

EXPORT SYSV_ABI int __NID(pthread_cond_init)(ScePthreadCond* cond, const ScePthreadCondattr* attr) {
  return POSIX_CALL(pthread::condInit(cond, attr, nullptr));
}

EXPORT SYSV_ABI int __NID(pthread_cond_broadcast)(ScePthreadCond* cond) {
  return POSIX_CALL(pthread::condBroadcast(cond));
}

EXPORT SYSV_ABI int __NID(pthread_cond_destroy)(ScePthreadCond* cond) {
  return POSIX_CALL(pthread::condDestroy(cond));
}

EXPORT SYSV_ABI int __NID(pthread_cond_signal)(ScePthreadCond* cond) {
  return POSIX_CALL(pthread::condSignal(cond));
}

EXPORT SYSV_ABI int __NID(pthread_cond_signalto_np)(ScePthreadCond* cond, ScePthread_obj obj) {
  return POSIX_CALL(pthread::condSignalto(cond, obj));
}

EXPORT SYSV_ABI int __NID(pthread_cond_timedwait)(ScePthreadCond* cond, ScePthreadMutex* mutex, const SceKernelTimespec* t) {
  return POSIX_CALL(pthread::condTimedwait(cond, mutex, t));
}

EXPORT SYSV_ABI int __NID(pthread_cond_wait)(ScePthreadCond* cond, ScePthreadMutex* mutex) {
  return POSIX_CALL(pthread::condWait(cond, mutex));
}

EXPORT SYSV_ABI int __NID(pthread_detach)(ScePthread_obj obj) {
  return POSIX_CALL(pthread::detach(obj));
}

EXPORT SYSV_ABI int __NID(pthread_equal)(ScePthread_obj thread1, ScePthread_obj thread2) {
  return POSIX_CALL(pthread::equal(thread1, thread2));
}

EXPORT SYSV_ABI int __NID(pthread_getcpuclockid)(ScePthread_obj thread, int* clock) {
  return POSIX_CALL(pthread::getcpuclockid(thread, clock));
}

EXPORT SYSV_ABI int __NID(pthread_join)(ScePthread_obj obj, void** value) {
  return POSIX_CALL(pthread::join(obj, value));
}

EXPORT SYSV_ABI int __NID(pthread_key_delete)(ScePthreadKey key) {
  return POSIX_CALL(pthread::keyDelete(key));
}

EXPORT SYSV_ABI int __NID(pthread_key_create)(ScePthreadKey* key, pthread_key_destructor_func_t destructor) {
  return POSIX_CALL(pthread::keyCreate(key, destructor));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_init)(ScePthreadMutexattr* attr) {
  return POSIX_CALL(pthread::mutexattrInit(attr));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_destroy)(ScePthreadMutexattr* attr) {
  return POSIX_CALL(pthread::mutexattrDestroy(attr));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_gettype)(ScePthreadMutexattr* attr, int* type) {
  return POSIX_CALL(pthread::mutexattrGettype(attr, type));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_settype)(ScePthreadMutexattr* attr, int type) {
  return POSIX_CALL(pthread::mutexattrSettype(attr, type));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_destroy)(ScePthreadMutex* mutex) {
  return POSIX_CALL(pthread::mutexDestroy(mutex));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_init)(ScePthreadMutex* mutex, const ScePthreadMutexattr* attr) {
  return POSIX_CALL(pthread::mutexInit(mutex, attr, nullptr));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_lock)(ScePthreadMutex* mutex) {
  return POSIX_CALL(pthread::mutexLock(mutex));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_trylock)(ScePthreadMutex* mutex) {
  return POSIX_CALL(pthread::mutexTrylock(mutex));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_timedlock)(ScePthreadMutex* mutex, const SceKernelTimespec* t) {
  return POSIX_CALL(pthread::mutexTimedlock(mutex, t));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_unlock)(ScePthreadMutex* mutex) {
  return POSIX_CALL(pthread::mutexUnlock(mutex));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_destroy)(ScePthreadRwlock* rwlock) {
  return POSIX_CALL(pthread::rwlockDestroy(rwlock));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_init)(ScePthreadRwlock* rwlock, const ScePthreadRwlockattr* attr) {
  return POSIX_CALL(pthread::rwlockInit(rwlock, attr, nullptr));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_rdlock)(ScePthreadRwlock* rwlock) noexcept {
  return POSIX_CALL(pthread::rwlockRdlock(rwlock));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_timedrdlock)(ScePthreadRwlock* rwlock, const SceKernelTimespec* t) {
  return POSIX_CALL(pthread::rwlockTimedrdlock(rwlock, t));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_timedwrlock)(ScePthreadRwlock* rwlock, const SceKernelTimespec* t) {
  return POSIX_CALL(pthread::rwlockTimedwrlock(rwlock, t));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_tryrdlock)(ScePthreadRwlock* rwlock) {
  return POSIX_CALL(pthread::rwlockTryrdlock(rwlock));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_trywrlock)(ScePthreadRwlock* rwlock) {
  return POSIX_CALL(pthread::rwlockTrywrlock(rwlock));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_unlock)(ScePthreadRwlock* rwlock) {
  return POSIX_CALL(pthread::rwlockUnlock(rwlock));
}

EXPORT SYSV_ABI int __NID(pthread_rwlock_wrlock)(ScePthreadRwlock* rwlock) {
  return POSIX_CALL(pthread::rwlockWrlock(rwlock));
}

EXPORT SYSV_ABI int __NID(pthread_rwlockattr_destroy)(ScePthreadRwlockattr* attr) {
  return POSIX_CALL(pthread::rwlockattrDestroy(attr));
}

EXPORT SYSV_ABI int __NID(pthread_rwlockattr_settype_np)(ScePthreadRwlockattr* attr, int type) {
  return POSIX_CALL(pthread::rwlockattrSettype(attr, type));
}

EXPORT SYSV_ABI int __NID(pthread_rwlockattr_init)(ScePthreadRwlockattr* attr) {
  return POSIX_CALL(pthread::rwlockattrInit(attr));
}

EXPORT SYSV_ABI int __NID(pthread_rwlockattr_gettype_np)(ScePthreadRwlockattr* attr, int* type) {
  return POSIX_CALL(pthread::rwlockattrGettype(attr, type));
}

EXPORT SYSV_ABI ScePthread_obj __NID(pthread_self)(void) {
  return pthread::getSelf();
}

EXPORT SYSV_ABI int __NID(pthread_setspecific)(ScePthreadKey key, const void* value) {
  return POSIX_CALL(pthread::setspecific(key, value));
}

EXPORT SYSV_ABI void* __NID(pthread_getspecific)(ScePthreadKey key) {
  return pthread::getspecific(key);
}

EXPORT SYSV_ABI int __NID(pthread_cancel)(ScePthread_obj obj) {
  return POSIX_CALL(pthread::cancel(obj));
}

EXPORT SYSV_ABI int __NID(pthread_setcancelstate)(int state, int* oldState) {
  return POSIX_CALL(pthread::setcancelstate(state, oldState));
}

EXPORT SYSV_ABI int __NID(pthread_setcanceltype)(int type, int* oldType) {
  return POSIX_CALL(pthread::setcanceltype(type, oldType));
}

EXPORT SYSV_ABI void __NID(pthread_testcancel)(void) {
  pthread::testCancel();
}

EXPORT SYSV_ABI int __NID(pthread_getprio)(ScePthread_obj obj, int* prio) {
  return POSIX_CALL(pthread::getprio(obj, prio));
}

EXPORT SYSV_ABI int __NID(pthread_setprio)(ScePthread_obj obj, int prio) {
  return POSIX_CALL(pthread::setprio(obj, prio));
}

EXPORT SYSV_ABI void __NID(pthread_yield)(void) {
  pthread::yield();
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_getprioceiling)(ScePthreadMutexattr* attr, int* prio) {
  return POSIX_CALL(pthread::mutexattrGetprioceiling(attr, prio));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_setprioceiling)(ScePthreadMutexattr* attr, int prio) {
  return POSIX_CALL(pthread::mutexattrSetprioceiling(attr, prio));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_getprioceiling)(ScePthreadMutex* mutex, int* prio) {
  return POSIX_CALL(pthread::mutexGetprioceiling(mutex, prio));
}

EXPORT SYSV_ABI int __NID(pthread_mutex_setprioceiling)(ScePthreadMutex* mutex, int prio, int* oldPrio) {
  return POSIX_CALL(pthread::mutexSetprioceiling(mutex, prio, oldPrio));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_getprotocol)(ScePthreadMutexattr* attr, int* protocol) {
  return POSIX_CALL(pthread::mutexattrGetprotocol(attr, protocol));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_setprotocol)(ScePthreadMutexattr* attr, int protocol) {
  return POSIX_CALL(pthread::mutexattrSetprotocol(attr, protocol));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_getpshared)(ScePthreadMutexattr* attr, int* pshared) {
  return POSIX_CALL(pthread::mutexattrGetpshared(attr, pshared));
}

EXPORT SYSV_ABI int __NID(pthread_mutexattr_setpshared)(ScePthreadMutexattr* attr, int pshared) {
  return POSIX_CALL(pthread::mutexattrSetpshared(attr, pshared));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getinheritsched)(const ScePthreadAttr* attr, SceInheritShed* inheritSched) noexcept {
  return POSIX_CALL(pthread::attrGetinheritsched(attr, inheritSched));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getschedparam)(const ScePthreadAttr* attr, SceSchedParam* param) noexcept {
  return POSIX_CALL(pthread::attrGetschedparam(attr, param));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getschedpolicy)(const ScePthreadAttr* attr, SceShedPolicy* policy) {
  return POSIX_CALL(pthread::attrGetschedpolicy(attr, policy));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setinheritsched)(ScePthreadAttr* attr, SceInheritShed inheritSched) {
  return POSIX_CALL(pthread::attrSetinheritsched(attr, inheritSched));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setschedparam)(ScePthreadAttr* attr, const SceSchedParam* param) {
  return POSIX_CALL(pthread::attrSetschedparam(attr, param));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setschedpolicy)(ScePthreadAttr* attr, SceShedPolicy policy) {
  return POSIX_CALL(pthread::attrSetschedpolicy(attr, policy));
}

EXPORT SYSV_ABI int __NID(pthread_getschedparam)(ScePthread_obj obj, int* policy, SceSchedParam* param) noexcept {
  return POSIX_CALL(pthread::getschedparam(obj, policy, param));
}

EXPORT SYSV_ABI int __NID(pthread_setschedparam)(ScePthread_obj obj, int policy, const SceSchedParam* param) {
  return POSIX_CALL(pthread::setschedparam(obj, policy, param));
}

EXPORT SYSV_ABI int __NID(pthread_attr_get_np)(ScePthread_obj obj, ScePthreadAttr* attr) {
  return POSIX_CALL(pthread::attrGet(obj, attr));
}

EXPORT SYSV_ABI int __NID(getpid)(void) {
  return POSIX_CALL(pthread::getthreadid());
}

EXPORT SYSV_ABI int __NID(pthread_once)(ScePthreadOnce once_control, pthread_once_init init_routine) {
  return POSIX_CALL(pthread::once(once_control, init_routine));
}

EXPORT SYSV_ABI int __NID(pthread_rename_np)(ScePthread_obj obj, const char* name) {
  return POSIX_CALL(pthread::rename(obj, name));
}

EXPORT SYSV_ABI int __NID(pthread_getname_np)(ScePthread_obj obj, char* name) {
  return POSIX_CALL(pthread::getName(obj, name));
}

EXPORT SYSV_ABI int __NID(pthread_attr_init)(ScePthreadAttr* attr) {
  return POSIX_CALL(pthread::attrInit(attr));
}

EXPORT SYSV_ABI void __NID(pthread_exit)(void* value) {
  pthread::exit(value);
}

EXPORT SYSV_ABI int __NID(pthread_create_name_np)(ScePthread_obj* obj, const ScePthreadAttr* attr, pthread_entry_func_t entry, void* arg, const char* name) {
  return POSIX_CALL(pthread::create(obj, attr, entry, arg, name));
}

EXPORT SYSV_ABI int __NID(pthread_create)(ScePthread_obj* obj, const ScePthreadAttr* attr, pthread_entry_func_t entry, void* arg) {
  return POSIX_CALL(pthread::create(obj, attr, entry, arg, nullptr));
}

EXPORT SYSV_ABI int __NID(pthread_attr_setscope)(ScePthreadAttr* attr, int flag) {
  return POSIX_CALL(pthread::attrSetscope(attr, flag));
}

EXPORT SYSV_ABI int __NID(pthread_attr_getscope)(ScePthreadAttr* attr, int* flag) {
  return POSIX_CALL(pthread::attrGetscope(attr, flag));
}

EXPORT SYSV_ABI void __NID(pthread_cleanup_push)(thread_clean_func_t func, void* arg) {
  pthread::cleanup_push(func, arg);
}

EXPORT SYSV_ABI void __NID(pthread_cleanup_pop)(int execute) {
  pthread::cleanup_pop(execute);
}

EXPORT SYSV_ABI int __NID(getpagesize)(void) {
  return memory::getpagesize();
}
}
