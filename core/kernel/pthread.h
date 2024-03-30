#pragma once
#include "pthread_types.h"
#include "utility/utility.h"

#include <modules_include/common.h>
#include <stdint.h>

struct PthreadAttrPrivate;
struct PthreadBarrierPrivate;
struct PthreadBarrierattrPrivate;
struct PthreadCondattrPrivate;
struct PthreadCondPrivate;
struct PthreadMutexPrivate;
struct PthreadMutexattrPrivate;
struct PthreadRwlockPrivate;
struct PthreadRwlockattrPrivate;
struct PthreadPrivate;
struct PthreadOnce;

using ScePthread_obj        = uint8_t*;
using ScePthreadAttr        = PthreadAttrPrivate*;
using ScePthreadBarrier     = PthreadBarrierPrivate*;
using ScePthreadBarrierattr = PthreadBarrierattrPrivate*;
using ScePthreadCondattr    = PthreadCondattrPrivate*;
using ScePthreadCond        = PthreadCondPrivate*;
using ScePthreadMutex       = PthreadMutexPrivate*;
using ScePthreadMutexattr   = PthreadMutexattrPrivate*;
using ScePthreadRwlock      = PthreadRwlockPrivate*;
using ScePthreadRwlockattr  = PthreadRwlockattrPrivate*;
using ScePthread            = PthreadPrivate*;
using ScePthreadOnce        = PthreadOnce*;
using ScePthreadKey         = int;

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

struct DTVKey;

namespace pthread {

__APICALL uint8_t*  getTLSStaticBlock(ScePthread_obj obj);
__APICALL uint64_t* getDTV(ScePthread_obj obj);

__APICALL int getThreadId();
__APICALL int getThreadId(ScePthread_obj obj);

__APICALL ScePthread_obj& getSelf();

__APICALL int attrDestroy(ScePthreadAttr* attr);
__APICALL int attrGetstack(const ScePthreadAttr* attr, void** stackAddr, size_t* stackSize);
__APICALL int attrGetstacksize(const ScePthreadAttr* attr, size_t* stackSize);
__APICALL int attrGetguardsize(const ScePthreadAttr* attr, size_t* guardSize);
__APICALL int attrGetstackaddr(const ScePthreadAttr* attr, void** stackAddr);
__APICALL int attrGetdetachstate(const ScePthreadAttr* attr, SceDetachState* state);
__APICALL int attrSetstacksize(ScePthreadAttr* attr, size_t stackSize);
__APICALL int attrSetguardsize(ScePthreadAttr* attr, size_t guardSize);
__APICALL int attrSetstack(ScePthreadAttr* attr, void* addr, size_t size);
__APICALL int attrSetstackaddr(ScePthreadAttr* attr, void* addr);
__APICALL int attrSetdetachstate(ScePthreadAttr* attr, SceDetachState state);

__APICALL int barrierDestroy(ScePthreadBarrier* barrier);
__APICALL int barrierInit(ScePthreadBarrier* barrier, const ScePthreadBarrierattr* attr, unsigned count, const char* name);
__APICALL int barrierWait(ScePthreadBarrier* barrier);

__APICALL int barrierattrDestroy(ScePthreadBarrierattr* barrier);
__APICALL int barrierattrInit(ScePthreadBarrierattr* barrier);
__APICALL int barrierattrGetpshared(ScePthreadBarrierattr* barrier, int* pshared);
__APICALL int barrierattrSetpshared(ScePthreadBarrierattr* barrier, int pshared);

__APICALL int condattrDestroy(ScePthreadCondattr* attr);
__APICALL int condattrInit(ScePthreadCondattr* attr);

__APICALL int condInit(ScePthreadCond* cond, const ScePthreadCondattr* attr, const char* name);
__APICALL int condBroadcast(ScePthreadCond* cond);
__APICALL int condDestroy(ScePthreadCond* cond);
__APICALL int condSignal(ScePthreadCond* cond);
__APICALL int condSignalto(ScePthreadCond* cond, ScePthread_obj obj);
__APICALL int condTimedwait(ScePthreadCond* cond, ScePthreadMutex* mutex, SceKernelUseconds usec);
__APICALL int condTimedwait(ScePthreadCond* cond, ScePthreadMutex* mutex, const SceKernelTimespec* t);
__APICALL int condWait(ScePthreadCond* cond, ScePthreadMutex* mutex);

__APICALL int detach(ScePthread_obj obj);
__APICALL int equal(ScePthread_obj thread1, ScePthread_obj thread2);

__APICALL int getcpuclockid(ScePthread_obj thread, int* clock);

__APICALL int join(ScePthread_obj obj, void** value);

__APICALL int keyDelete(ScePthreadKey key);
__APICALL int keyCreate(ScePthreadKey* key, pthread_key_destructor_func_t destructor);

__APICALL int mutexattrInit(ScePthreadMutexattr* attr);
__APICALL int mutexattrDestroy(ScePthreadMutexattr* attr);
__APICALL int mutexattrGettype(ScePthreadMutexattr* attr, int* type);
__APICALL int mutexattrSettype(ScePthreadMutexattr* attr, int type);

__APICALL int mutexDestroy(ScePthreadMutex* mutex);
__APICALL int mutexInit(ScePthreadMutex* mutex, const ScePthreadMutexattr* attr, const char* name);
__APICALL int mutexLock(ScePthreadMutex* mutex);
__APICALL int mutexTrylock(ScePthreadMutex* mutex);
__APICALL int mutexTimedlock(ScePthreadMutex* mutex, SceKernelUseconds usec);
__APICALL int mutexTimedlock(ScePthreadMutex* mutex, const SceKernelTimespec* t);
__APICALL int mutexUnlock(ScePthreadMutex* mutex);

__APICALL int rwlockDestroy(ScePthreadRwlock* rwlock);
__APICALL int rwlockInit(ScePthreadRwlock* rwlock, const ScePthreadRwlockattr* attr, const char* name);

__APICALL int rwlockRdlock(ScePthreadRwlock* rwlock);
__APICALL int rwlockTimedrdlock(ScePthreadRwlock* rwlock, SceKernelUseconds usec);
__APICALL int rwlockTimedrdlock(ScePthreadRwlock* rwlock, const SceKernelTimespec* t);
__APICALL int rwlockTimedwrlock(ScePthreadRwlock* rwlock, SceKernelUseconds usec);
__APICALL int rwlockTimedwrlock(ScePthreadRwlock* rwlock, const SceKernelTimespec* t);
__APICALL int rwlockTryrdlock(ScePthreadRwlock* rwlock);
__APICALL int rwlockTrywrlock(ScePthreadRwlock* rwlock);
__APICALL int rwlockUnlock(ScePthreadRwlock* rwlock);
__APICALL int rwlockWrlock(ScePthreadRwlock* rwlock);

__APICALL int rwlockattrDestroy(ScePthreadRwlockattr* attr);
__APICALL int rwlockattrSettype(ScePthreadRwlockattr* attr, int type);
__APICALL int rwlockattrInit(ScePthreadRwlockattr* attr);
__APICALL int rwlockattrGettype(ScePthreadRwlockattr* attr, int* type);

__APICALL int   setspecific(ScePthreadKey key, const void* value);
__APICALL void* getspecific(ScePthreadKey key);

__APICALL int cancel(ScePthread_obj obj);

__APICALL int setcancelstate(int state, int* oldState);
__APICALL int setcanceltype(int type, int* oldType);

__APICALL void testCancel(void);

__APICALL int getprio(ScePthread_obj obj, int* prio);
__APICALL int setprio(ScePthread_obj obj, int prio);

__APICALL void yield(void);

__APICALL int mutexattrGetprioceiling(ScePthreadMutexattr* attr, int* prio);
__APICALL int mutexattrSetprioceiling(ScePthreadMutexattr* attr, int prio);
__APICALL int mutexGetprioceiling(ScePthreadMutex* mutex, int* prio);
__APICALL int mutexSetprioceiling(ScePthreadMutex* mutex, int prio, int* oldPrio);
__APICALL int mutexattrGetprotocol(ScePthreadMutexattr* attr, int* protocol);
__APICALL int mutexattrSetprotocol(ScePthreadMutexattr* attr, int protocol);

__APICALL int attrGetinheritsched(const ScePthreadAttr* attr, SceInheritShed* inheritSched);
__APICALL int attrGetschedparam(const ScePthreadAttr* attr, SceSchedParam* param);
__APICALL int attrGetschedpolicy(const ScePthreadAttr* attr, SceShedPolicy* policy);
__APICALL int attrSetinheritsched(ScePthreadAttr* attr, SceInheritShed inheritSched);
__APICALL int attrSetschedparam(ScePthreadAttr* attr, const SceSchedParam* param);
__APICALL int attrSetschedpolicy(ScePthreadAttr* attr, SceShedPolicy policy);

__APICALL int getschedparam(ScePthread_obj obj, int* policy, SceSchedParam* param);
__APICALL int setschedparam(ScePthread_obj obj, int policy, const SceSchedParam* param);

__APICALL int attrGet(ScePthread_obj obj, ScePthreadAttr* attr);

__APICALL int attrGetaffinity(const ScePthreadAttr* attr, SceKernelCpumask* mask);
__APICALL int attrSetaffinity(ScePthreadAttr* attr, const SceKernelCpumask mask);

__APICALL int getaffinity(ScePthread_obj obj, SceKernelCpumask* mask);
__APICALL int setaffinity(ScePthread_obj obj, const SceKernelCpumask mask);

__APICALL int getthreadid(void);
__APICALL int once(ScePthreadOnce once_control, pthread_once_init init_routine);

__APICALL int rename(ScePthread_obj obj, const char* name);
__APICALL int getName(ScePthread_obj obj, char* name);

__APICALL void setThreadDtors(thread_dtors_func_t dtors);

__APICALL int attrInit(ScePthreadAttr* attr);

__APICALL void exit(void* value);
__APICALL void raise(ScePthread_obj obj, void* callback, int signo);

__APICALL int create(ScePthread_obj* obj, const ScePthreadAttr* attr, pthread_entry_func_t entry, void* arg, const char* name);

__APICALL int attrSetscope(ScePthreadAttr* attr, int flag);
__APICALL int attrGetscope(ScePthreadAttr* attr, int* flag);

__APICALL void cxa_finalize(void* /*p*/);

__APICALL void cleanup_push(thread_clean_func_t func, void* arg);
__APICALL void cleanup_pop(int execute);

} // namespace pthread

#undef __APICALL