#include "core/kernel/pthread.h"

#include "common.h"
#include "logging.h"
#include "types.h"

#include <stdarg.h>

LOG_DEFINE_MODULE(pthread);

extern "C" {
EXPORT SYSV_ABI int scePthreadAttrDestroy(ScePthreadAttr* attr) {
  return pthread::attrDestroy(attr);
}

EXPORT SYSV_ABI int scePthreadAttrGetstack(const ScePthreadAttr* attr, void** stackAddr, size_t* stackSize) {
  return pthread::attrGetstack(attr, stackAddr, stackSize);
}

EXPORT SYSV_ABI int scePthreadAttrGetstacksize(const ScePthreadAttr* attr, size_t* stackSize) {
  return pthread::attrGetstacksize(attr, stackSize);
}

EXPORT SYSV_ABI int scePthreadAttrGetguardsize(const ScePthreadAttr* attr, size_t* guardSize) {
  return pthread::attrGetguardsize(attr, guardSize);
}

EXPORT SYSV_ABI int scePthreadAttrGetstackaddr(const ScePthreadAttr* attr, void** stackAddr) {
  return pthread::attrGetstackaddr(attr, stackAddr);
}

EXPORT SYSV_ABI int scePthreadAttrGetdetachstate(const ScePthreadAttr* attr, SceDetachState* state) {
  return pthread::attrGetdetachstate(attr, state);
}

EXPORT SYSV_ABI int scePthreadAttrSetstacksize(ScePthreadAttr* attr, size_t stackSize) {
  return pthread::attrSetstacksize(attr, stackSize);
}

EXPORT SYSV_ABI int scePthreadAttrSetguardsize(ScePthreadAttr* attr, size_t guardSize) {
  return pthread::attrSetguardsize(attr, guardSize);
}

EXPORT SYSV_ABI int scePthreadAttrSetstack(ScePthreadAttr* attr, void* addr, size_t size) {
  return pthread::attrSetstack(attr, addr, size);
}

EXPORT SYSV_ABI int scePthreadAttrSetstackaddr(ScePthreadAttr* attr, void* addr) {
  return pthread::attrSetstackaddr(attr, addr);
}

EXPORT SYSV_ABI int scePthreadAttrSetdetachstate(ScePthreadAttr* attr, SceDetachState state) {
  return pthread::attrSetdetachstate(attr, state);
}

EXPORT SYSV_ABI int scePthreadBarrierDestroy(ScePthreadBarrier* barrier) {
  return pthread::barrierDestroy(barrier);
}

EXPORT SYSV_ABI int scePthreadBarrierInit(ScePthreadBarrier* barrier, const ScePthreadBarrierattr* attr, unsigned count, const char* name) {
  return pthread::barrierInit(barrier, attr, count, name);
}

EXPORT SYSV_ABI int scePthreadBarrierWait(ScePthreadBarrier* barrier) {
  return pthread::barrierWait(barrier);
}

EXPORT SYSV_ABI int scePthreadBarrierattrDestroy(ScePthreadBarrierattr* attr) {
  return pthread::barrierattrDestroy(attr);
}

EXPORT SYSV_ABI int scePthreadBarrierattrInit(ScePthreadBarrierattr* attr) {
  return pthread::barrierattrInit(attr);
}

EXPORT SYSV_ABI int scePthreadBarrierattrGetpshared(ScePthreadBarrierattr* barrier, int* pshared) {
  return pthread::barrierattrGetpshared(barrier, pshared);
}

EXPORT SYSV_ABI int scePthreadBarrierattrSetpshared(ScePthreadBarrierattr* barrier, int pshared) {
  return pthread::barrierattrSetpshared(barrier, pshared);
}

EXPORT SYSV_ABI int scePthreadCondattrDestroy(ScePthreadCondattr* attr) {
  return pthread::condattrDestroy(attr);
}

EXPORT SYSV_ABI int scePthreadCondattrInit(ScePthreadCondattr* attr) {
  return pthread::condattrInit(attr);
}

EXPORT SYSV_ABI int scePthreadCondInit(ScePthreadCond* cond, const ScePthreadCondattr* attr, const char* name) {
  return pthread::condInit(cond, attr, name);
}

EXPORT SYSV_ABI int scePthreadCondBroadcast(ScePthreadCond* cond) {
  return pthread::condBroadcast(cond);
}

EXPORT SYSV_ABI int scePthreadCondDestroy(ScePthreadCond* cond) {
  return pthread::condDestroy(cond);
}

EXPORT SYSV_ABI int scePthreadCondSignal(ScePthreadCond* cond) {
  return pthread::condSignal(cond);
}

EXPORT SYSV_ABI int scePthreadCondSignalto(ScePthreadCond* cond, ScePthread_obj obj) {
  return pthread::condSignalto(cond, obj);
}

EXPORT SYSV_ABI int scePthreadCondTimedwait(ScePthreadCond* cond, ScePthreadMutex* mutex, SceKernelUseconds usec) {
  return pthread::condTimedwait(cond, mutex, usec);
}

EXPORT SYSV_ABI int PthreadCondTimedwait(ScePthreadCond* cond, ScePthreadMutex* mutex, const SceKernelTimespec* t) {
  return pthread::condTimedwait(cond, mutex, t);
}

EXPORT SYSV_ABI int scePthreadCondWait(ScePthreadCond* cond, ScePthreadMutex* mutex) {
  return pthread::condWait(cond, mutex);
}

EXPORT SYSV_ABI int scePthreadDetach(ScePthread_obj obj) {
  return pthread::detach(obj);
}

EXPORT SYSV_ABI int scePthreadEqual(ScePthread_obj thread1, ScePthread_obj thread2) {
  return pthread::equal(thread1, thread2);
}

EXPORT SYSV_ABI int scePthreadGetcpuclockid(ScePthread_obj thread, int* clock) {
  return pthread::getcpuclockid(thread, clock);
}

EXPORT SYSV_ABI int scePthreadJoin(ScePthread_obj obj, void** value) {
  return pthread::join(obj, value);
}

EXPORT SYSV_ABI int scePthreadKeyDelete(ScePthreadKey key) {
  return pthread::keyDelete(key);
}

EXPORT SYSV_ABI int scePthreadKeyCreate(ScePthreadKey* key, pthread_key_destructor_func_t destructor) {
  return pthread::keyCreate(key, destructor);
}

EXPORT SYSV_ABI int scePthreadMutexattrInit(ScePthreadMutexattr* attr) {
  return pthread::mutexattrInit(attr);
}

EXPORT SYSV_ABI int scePthreadMutexattrDestroy(ScePthreadMutexattr* attr) {
  return pthread::mutexattrDestroy(attr);
}

EXPORT SYSV_ABI int scePthreadMutexattrGettype(ScePthreadMutexattr* attr, int* type) {
  return pthread::mutexattrGettype(attr, type);
}

EXPORT SYSV_ABI int scePthreadMutexattrSettype(ScePthreadMutexattr* attr, int type) {
  return pthread::mutexattrSettype(attr, type);
}

EXPORT SYSV_ABI int scePthreadMutexDestroy(ScePthreadMutex* mutex) {
  return pthread::mutexDestroy(mutex);
}

EXPORT SYSV_ABI int scePthreadMutexInit(ScePthreadMutex* mutex, const ScePthreadMutexattr* attr, const char* name) {
  return pthread::mutexInit(mutex, attr, name);
}

EXPORT SYSV_ABI int scePthreadMutexLock(ScePthreadMutex* mutex) {
  return pthread::mutexLock(mutex);
}

EXPORT SYSV_ABI int scePthreadMutexTrylock(ScePthreadMutex* mutex) {
  return pthread::mutexTrylock(mutex);
}

EXPORT SYSV_ABI int scePthreadMutexTimedlock(ScePthreadMutex* mutex, SceKernelUseconds usec) {
  return pthread::mutexTimedlock(mutex, usec);
}

EXPORT SYSV_ABI int PthreadMutexTimedlock(ScePthreadMutex* mutex, const SceKernelTimespec* t) {
  return pthread::mutexTimedlock(mutex, t);
}

EXPORT SYSV_ABI int scePthreadMutexUnlock(ScePthreadMutex* mutex) {
  return pthread::mutexUnlock(mutex);
}

EXPORT SYSV_ABI int scePthreadRwlockDestroy(ScePthreadRwlock* rwlock) {
  return pthread::rwlockDestroy(rwlock);
}

EXPORT SYSV_ABI int scePthreadRwlockInit(ScePthreadRwlock* rwlock, const ScePthreadRwlockattr* attr, const char* name) {
  return pthread::rwlockInit(rwlock, attr, name);
}

EXPORT SYSV_ABI int scePthreadRwlockRdlock(ScePthreadRwlock* rwlock) noexcept {
  return pthread::rwlockRdlock(rwlock);
}

EXPORT SYSV_ABI int scePthreadRwlockTimedrdlock(ScePthreadRwlock* rwlock, SceKernelUseconds usec) {
  return pthread::rwlockTimedrdlock(rwlock, usec);
}

EXPORT SYSV_ABI int PthreadRwlockTimedrdlock(ScePthreadRwlock* rwlock, const SceKernelTimespec* t) {
  return pthread::rwlockTimedrdlock(rwlock, t);
}

EXPORT SYSV_ABI int scePthreadRwlockTimedwrlock(ScePthreadRwlock* rwlock, SceKernelUseconds usec) {
  return pthread::rwlockTimedwrlock(rwlock, usec);
}

EXPORT SYSV_ABI int PthreadRwlockTimedwrlock(ScePthreadRwlock* rwlock, const SceKernelTimespec* t) {
  return pthread::rwlockTimedwrlock(rwlock, t);
}

EXPORT SYSV_ABI int scePthreadRwlockTryrdlock(ScePthreadRwlock* rwlock) {
  return pthread::rwlockTryrdlock(rwlock);
}

EXPORT SYSV_ABI int scePthreadRwlockTrywrlock(ScePthreadRwlock* rwlock) {
  return pthread::rwlockTrywrlock(rwlock);
}

EXPORT SYSV_ABI int scePthreadRwlockUnlock(ScePthreadRwlock* rwlock) {
  return pthread::rwlockUnlock(rwlock);
}

EXPORT SYSV_ABI int scePthreadRwlockWrlock(ScePthreadRwlock* rwlock) {
  return pthread::rwlockWrlock(rwlock);
}

EXPORT SYSV_ABI int scePthreadRwlockattrDestroy(ScePthreadRwlockattr* attr) {
  return pthread::rwlockattrDestroy(attr);
}

EXPORT SYSV_ABI int scePthreadRwlockattrSettype(ScePthreadRwlockattr* attr, int type) {
  return pthread::rwlockattrSettype(attr, type);
}

EXPORT SYSV_ABI int scePthreadRwlockattrInit(ScePthreadRwlockattr* attr) {
  return pthread::rwlockattrInit(attr);
}

EXPORT SYSV_ABI int scePthreadRwlockattrGettype(ScePthreadRwlockattr* attr, int* type) {
  return pthread::rwlockattrGettype(attr, type);
}

EXPORT SYSV_ABI ScePthread_obj scePthreadSelf(void) {
  return pthread::getSelf();
}

EXPORT SYSV_ABI int scePthreadSetspecific(ScePthreadKey key, const void* value) {
  return pthread::setspecific(key, value);
}

EXPORT SYSV_ABI void* scePthreadGetspecific(ScePthreadKey key) {
  return pthread::getspecific(key);
}

EXPORT SYSV_ABI int scePthreadCancel(ScePthread_obj obj) {
  return pthread::cancel(obj);
}

EXPORT SYSV_ABI int scePthreadSetcancelstate(int state, int* oldState) {
  return pthread::setcancelstate(state, oldState);
}

EXPORT SYSV_ABI int scePthreadSetcanceltype(int type, int* oldType) {
  return pthread::setcanceltype(type, oldType);
}

EXPORT SYSV_ABI void scePthreadTestcancel(void) {
  return pthread::testCancel();
}

EXPORT SYSV_ABI int scePthreadGetprio(ScePthread_obj obj, int* prio) {
  return pthread::getprio(obj, prio);
}

EXPORT SYSV_ABI int scePthreadSetprio(ScePthread_obj obj, int prio) {
  return pthread::setprio(obj, prio);
}

EXPORT SYSV_ABI void scePthreadYield(void) {
  return pthread::yield();
}

EXPORT SYSV_ABI int scePthreadMutexattrGetprioceiling(ScePthreadMutexattr* attr, int* prio) {
  return pthread::mutexattrGetprioceiling(attr, prio);
}

EXPORT SYSV_ABI int scePthreadMutexattrSetprioceiling(ScePthreadMutexattr* attr, int prio) {
  return pthread::mutexattrSetprioceiling(attr, prio);
}

EXPORT SYSV_ABI int scePthreadMutexGetprioceiling(ScePthreadMutex* mutex, int* prio) {
  return pthread::mutexGetprioceiling(mutex, prio);
}

EXPORT SYSV_ABI int scePthreadMutexSetprioceiling(ScePthreadMutex* mutex, int prio, int* oldPrio) {
  return pthread::mutexSetprioceiling(mutex, prio, oldPrio);
}

EXPORT SYSV_ABI int scePthreadMutexattrGetprotocol(ScePthreadMutexattr* attr, int* protocol) {
  return pthread::mutexattrGetprotocol(attr, protocol);
}

EXPORT SYSV_ABI int scePthreadMutexattrSetprotocol(ScePthreadMutexattr* attr, int protocol) {
  return pthread::mutexattrSetprotocol(attr, protocol);
}

EXPORT SYSV_ABI int scePthreadAttrGetinheritsched(const ScePthreadAttr* attr, SceInheritShed* inheritSched) noexcept {
  return pthread::attrGetinheritsched(attr, inheritSched);
}

EXPORT SYSV_ABI int scePthreadAttrGetschedparam(const ScePthreadAttr* attr, SceSchedParam* param) noexcept {
  return pthread::attrGetschedparam(attr, param);
}

EXPORT SYSV_ABI int scePthreadAttrGetschedpolicy(const ScePthreadAttr* attr, SceShedPolicy* policy) {
  return pthread::attrGetschedpolicy(attr, policy);
}

EXPORT SYSV_ABI int scePthreadAttrSetinheritsched(ScePthreadAttr* attr, SceInheritShed inheritSched) {
  return pthread::attrSetinheritsched(attr, inheritSched);
}

EXPORT SYSV_ABI int scePthreadAttrSetschedparam(ScePthreadAttr* attr, const SceSchedParam* param) {
  return pthread::attrSetschedparam(attr, param);
}

EXPORT SYSV_ABI int scePthreadAttrSetschedpolicy(ScePthreadAttr* attr, SceShedPolicy policy) {
  return pthread::attrSetschedpolicy(attr, policy);
}

EXPORT SYSV_ABI int scePthreadGetschedparam(ScePthread_obj obj, int* policy, SceSchedParam* param) noexcept {
  return pthread::getschedparam(obj, policy, param);
}

EXPORT SYSV_ABI int scePthreadSetschedparam(ScePthread_obj obj, int policy, const SceSchedParam* param) {
  return pthread::setschedparam(obj, policy, param);
}

EXPORT SYSV_ABI int scePthreadAttrGet(ScePthread_obj obj, ScePthreadAttr* attr) {
  return pthread::attrGet(obj, attr);
}

EXPORT SYSV_ABI int scePthreadAttrGetaffinity(const ScePthreadAttr* attr, SceKernelCpumask* mask) {
  return pthread::attrGetaffinity(attr, mask);
}

EXPORT SYSV_ABI int scePthreadAttrSetaffinity(ScePthreadAttr* attr, const SceKernelCpumask mask) {
  return pthread::attrSetaffinity(attr, mask);
}

EXPORT SYSV_ABI int scePthreadGetaffinity(ScePthread_obj obj, SceKernelCpumask* mask) {
  return pthread::getaffinity(obj, mask);
}

EXPORT SYSV_ABI int scePthreadSetaffinity(ScePthread_obj obj, const SceKernelCpumask mask) {
  return pthread::setaffinity(obj, mask);
}

EXPORT SYSV_ABI int scePthreadGetthreadid(void) {
  return pthread::getthreadid();
}

EXPORT SYSV_ABI int scePthreadRename(ScePthread_obj obj, const char* name) {
  return pthread::rename(obj, name);
}

EXPORT SYSV_ABI int scePthreadOnce(ScePthreadOnce once_control, pthread_once_init init_routine) {
  return pthread::once(once_control, init_routine);
}

EXPORT SYSV_ABI int scePthreadGetname(ScePthread_obj obj, char* name) {
  return pthread::getName(obj, name);
}

EXPORT SYSV_ABI void _sceKernelSetThreadDtors(thread_dtors_func_t dtors) {
  return pthread::setThreadDtors(dtors);
}

EXPORT SYSV_ABI int scePthreadAttrInit(ScePthreadAttr* attr) {
  return pthread::attrInit(attr);
}

EXPORT SYSV_ABI void scePthreadExit(void* value) {
  return pthread::exit(value);
}

EXPORT SYSV_ABI int scePthreadCreate(ScePthread_obj* obj, const ScePthreadAttr* attr, pthread_entry_func_t entry, void* arg, const char* name) {
  return pthread::create(obj, attr, entry, arg, name);
}

EXPORT SYSV_ABI int scePthreadAttrSetscope(ScePthreadAttr* attr, int flag) {
  return pthread::attrSetscope(attr, flag);
}

EXPORT SYSV_ABI int scePthreadAttrGetscope(ScePthreadAttr* attr, int* flag) {
  return pthread::attrGetscope(attr, flag);
}

EXPORT SYSV_ABI void __NID(__pthread_cleanup_push_imp)(thread_clean_func_t func, void* arg, SceCleanInfo* info) {
  // fake it
  info->func   = func;
  info->arg    = arg;
  info->onHeap = 0;

  pthread::cleanup_push(func, arg);
}

EXPORT SYSV_ABI void __NID(__pthread_cleanup_pop_imp)(int execute) {
  pthread::cleanup_pop(execute);
}
}
