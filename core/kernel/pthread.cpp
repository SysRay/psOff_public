
#define __APICALL_EXPORT
#include "pthread.h"
#undef __APICALL_EXPORT

#include "logging.h"
#include "modules_include/common.h"
#include "pthread_intern.h"

#define __APICALL_IMPORT
#include "core/fileManager/fileManager.h"
#include "core/imports/imports_runtime.h"
#include "core/timer/timer.h"
#undef __APICALL_IMPORT

#include <assert.h>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/thread.hpp>
#include <intrin.h>
#include <memory>
#include <unordered_set>
#include <windows.h>

LOG_DEFINE_MODULE(pthread)

namespace {
NT_TIB* getTIB() {
#ifdef _M_IX86
  return (NT_TIB*)__readfsdword(0x18);
#elif _M_AMD64
  return (NT_TIB*)__readgsqword(0x30);
#else
#error unsupported architecture
#endif
}

struct PImpl {
  boost::mutex mutex;
  PImpl() = default;
};

PImpl* getData() {
  static PImpl data;
  return &data;
}

void setThreadPrio(boost::thread::native_handle_type nativeH, int prio) {
  int pr = 0;
  if (prio <= 478) {
    int pr = 0;
    pr     = +2;
  } else if (prio >= 733) {
    pr = -2;
  }

  SetThreadPriority(nativeH, pr);
}

auto getTimeDuration(SceKernelTimespec const* t) {
  auto              now = boost::chrono::high_resolution_clock::now();
  SceKernelTimespec tp;
  tp.tv_sec  = boost::chrono::time_point_cast<boost::chrono::seconds>(now).time_since_epoch().count();
  tp.tv_nsec = boost::chrono::time_point_cast<boost::chrono::nanoseconds>(now).time_since_epoch().count() % 1000000000;

  auto diffSec  = t->tv_sec - tp.tv_sec;
  auto diffNsec = t->tv_nsec - tp.tv_nsec;
  return boost::chrono::duration(boost::chrono::seconds(diffSec) + boost::chrono::nanoseconds(diffNsec));
}

static std::atomic<SceCancelState> g_cancelState = SceCancelState::DISABLE;
static std::atomic<SceCancelType>  g_cancelType  = SceCancelType::DEFERRED;

auto getThreadDtors() {
  static thread_dtors_func_t obj = nullptr;
  return &obj;
}

ScePthread getPthread(ScePthread_obj obj) {
  uint64_t pthreadOffset = ((uint64_t const*)obj)[0];
  return (ScePthread)&obj[pthreadOffset];
}

int threadCounter() {
  static int counter = 0;
  return ++counter;
}

size_t mutexCounter() {
  static int counter = 0;
  return ++counter;
}

void initTLS(ScePthread_obj obj) {
  auto pthread    = getPthread(obj);
  pthread->dtv[0] = 0;
  std::fill(&pthread->dtv[1], &pthread->dtv[DTV_SIZE - 2], 0); // skip mainprog
  pthread->dtv[DTV_SIZE - 1] = XSAVE_CHK_GUARD;
  accessRuntimeExport()->initTLS(obj); // reset tls
}

void init_pThread() {
  auto pimpl = getData();
}
} // namespace

namespace pthread {
int attrDestroy(ScePthreadAttr* attr) {
  delete *attr;
  *attr = nullptr;
  return Ok;
}

int attrGetstack(const ScePthreadAttr* attr, void** stackAddr, size_t* stackSize) {
  if (attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  if (stackAddr != nullptr) *stackAddr = (*attr)->getStackAddr();
  if (stackSize != nullptr) *stackSize = (*attr)->getStackSize();
  return Ok;
}

int attrGetstacksize(const ScePthreadAttr* attr, size_t* stackSize) {
  if (stackSize == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *stackSize = (*attr)->getStackSize();
  return Ok;
}

int attrGetguardsize(const ScePthreadAttr* attr, size_t* guardSize) {
  if (guardSize == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *guardSize = (*attr)->getGuardSize();
  return Ok;
}

int attrGetstackaddr(const ScePthreadAttr* attr, void** stackAddr) {
  if (stackAddr == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }
  *stackAddr = (*attr)->getStackAddr();
  return Ok;
}

int attrGetdetachstate(const ScePthreadAttr* attr, SceDetachState* state) {
  if (state == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *state = (*attr)->getDetachState();
  return Ok;
}

int attrSetstacksize(ScePthreadAttr* attr, size_t stackSize) {
  if (stackSize == 0 || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setStackSize(stackSize);
  return Ok;
}

int attrSetguardsize(ScePthreadAttr* attr, size_t guardSize) {
  if (attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setGuardSize(guardSize);
  return Ok;
}

int attrSetstack(ScePthreadAttr* attr, void* addr, size_t size) {
  LOG_USE_MODULE(pthread);
  LOG_DEBUG(L"Thread Stack 0x%08llx addr:0x%08llx size:0x%08llx", (uint64_t)attr, addr, size);

  if (addr == nullptr || size == 0 || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setStackAddr(addr);
  (*attr)->setStackSize(size);

  return Ok;
}

int attrSetstackaddr(ScePthreadAttr* attr, void* addr) {
  if (addr == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }
  (*attr)->setStackAddr(addr);
  return Ok;
}

int attrSetdetachstate(ScePthreadAttr* attr, SceDetachState state) {
  if (attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setDetachState(state);
  return Ok;
}

int barrierDestroy(ScePthreadBarrier* barrier) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int barrierInit(ScePthreadBarrier* barrier, const ScePthreadBarrierattr* attr, unsigned count, const char* name) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int barrierWait(ScePthreadBarrier* barrier) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int barrierattrDestroy(ScePthreadBarrierattr* barrier) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int barrierattrInit(ScePthreadBarrierattr* barrier) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int barrierattrGetpshared(ScePthreadBarrierattr* barrier, int* pshared) {
  if (barrier != nullptr) {
    *pshared = (*barrier)->pshared;
  } else {
    return getErr(ErrCode::_EACCES);
  }
  return Ok;
}

int barrierattrSetpshared(ScePthreadBarrierattr* barrier, int pshared) {
  if (barrier != nullptr) {
    (*barrier)->pshared = pshared;
  } else {
    return getErr(ErrCode::_EACCES);
  }
  return Ok;
}

int condattrDestroy(ScePthreadCondattr* attr) {
  delete *attr;
  *attr = nullptr;
  return Ok;
}

int condattrInit(ScePthreadCondattr* attr) {
  *attr = new PthreadCondattrPrivate();
  return Ok;
}

auto condInit_intern(const char* name) {
  auto cond = new PthreadCondPrivate();
  if (name != nullptr) cond->name = name;
  return cond;
}

int condInit(ScePthreadCond* cond, const ScePthreadCondattr* attr, const char* name) {
  if (cond == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }
  *cond = condInit_intern(name);

  return Ok;
}

static int checkCondInit(ScePthreadCond* cond) {
  if (*cond == nullptr) {
    static boost::mutex mutexInt;
    // Is Static Object -> init
    // race condition on first init -> protect
    boost::unique_lock lock(mutexInt);
    if (*cond == nullptr) *cond = condInit_intern(nullptr);
  }
  return Ok;
}

int condBroadcast(ScePthreadCond* cond) {
  if (int res = checkCondInit(cond); res != Ok) return res;

  (*cond)->p.notify_all();
  LOG_USE_MODULE(pthread);
  // LOG_DEBUG(L"Broadcast: %S", (*cond)->name.c_str());
  return Ok;
}

int condDestroy(ScePthreadCond* cond) {
  if (cond == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  delete *cond;
  *cond = nullptr;
  return Ok;
}

int condSignal(ScePthreadCond* cond) {
  if (int res = checkCondInit(cond); res != Ok) return res;
  LOG_USE_MODULE(pthread);
  // LOG_DEBUG(L"Cond: %S", (*cond)->name.c_str());
  (*cond)->p.notify_one();
  return Ok;
}

int condSignalto(ScePthreadCond* cond, ScePthread_obj obj) {
  if (int res = checkCondInit(cond); res != Ok) return res;

  (*cond)->p.notify_all();
  return Ok;
}

int condTimedwait(ScePthreadCond* cond, ScePthreadMutex* mutex, SceKernelUseconds usec) {
  if (int res = checkCondInit(cond); res != Ok) return res;

  LOG_USE_MODULE(pthread);
  // LOG_DEBUG(L"->Cond: %S mutex:%d", (*cond)->name.c_str(), (*mutex)->id);

  auto ret = (*cond)->p.do_wait_until((*mutex)->p, boost::detail::internal_platform_clock::now() + boost::chrono::microseconds(usec))
                 ? Ok
                 : getErr(ErrCode::_ETIMEDOUT);
  // LOG_DEBUG(L"<-Cond: %S", (*cond)->name.c_str());
  return ret;
}

int condTimedwait(ScePthreadCond* cond, ScePthreadMutex* mutex, const SceKernelTimespec* t) {
  if (int res = checkCondInit(cond); res != Ok) return res;

  LOG_USE_MODULE(pthread);
  // LOG_DEBUG(L"->Cond: %S mutex:%d", (*cond)->name.c_str(), (*mutex)->id);

  auto ret = (*cond)->p.do_wait_until((*mutex)->p, boost::detail::internal_platform_clock::now() + getTimeDuration(t)) ? Ok : getErr(ErrCode::_ETIMEDOUT);
  // LOG_DEBUG(L"<-Cond: %S", (*cond)->name.c_str());
  return ret;
}

int condWait(ScePthreadCond* cond, ScePthreadMutex* mutex) {
  if (int res = checkCondInit(cond); res != Ok) return res;

  LOG_USE_MODULE(pthread);
  // LOG_DEBUG(L"->Cond: %S mutex:%d", (*cond)->name.c_str(), (*mutex)->id);
  (*cond)->p.do_wait_until((*mutex)->p, boost::detail::internal_platform_timepoint::getMax());

  // LOG_DEBUG(L"<-Cond: %S", (*cond)->name.c_str());
  return Ok;
}

int detach(ScePthread_obj obj) {
  if (obj == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto thread = getPthread(obj);

  LOG_USE_MODULE(pthread);
  LOG_INFO(L"Detach :%d", thread->unique_id);
  thread->detached = true;
  // thread->p.detach(); // Signaling (raiseSignal) doesnt work when detached
  return Ok;
}

int equal(ScePthread_obj thread1, ScePthread_obj thread2) {
  if (thread1 == nullptr || thread2 == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto t1 = getPthread(thread1);
  auto t2 = getPthread(thread2);
  return t1->p == t2->p;
}

int getcpuclockid(ScePthread_obj thread, int* clock) {
  return 0; // todo
}

int join(ScePthread_obj obj, void** value) {
  if (obj == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto thread = getPthread(obj);
  thread->p.join();

  LOG_USE_MODULE(pthread);
  LOG_DEBUG(L"Delete thread:%d", thread->unique_id);
  // Cleanup thread
  thread->~PthreadPrivate();
  delete[] obj;
  // -
  return Ok;
}

int keyDelete(ScePthreadKey key) {
  accessRuntimeExport()->deleteTLSKey(key);
  return Ok;
}

int keyCreate(ScePthreadKey* key, pthread_key_destructor_func_t destructor) {
  if (key == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *key = accessRuntimeExport()->createTLSKey((void*)destructor);
  if (*key < 0) {
    return getErr(ErrCode::_EAGAIN);
  }

  return Ok;
}

int mutexattrInit(ScePthreadMutexattr* attr) {
  LOG_USE_MODULE(pthread);

  *attr = new PthreadMutexattrPrivate();
  // LOG_TRACE(L"->mutex_attr 0x%08llx", (uint64_t)attr);
  return Ok;
}

int mutexattrDestroy(ScePthreadMutexattr* attr) {
  if (attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  // LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"<-mutex_attr 0x%08llx", (uint64_t)attr);
  delete *attr;
  *attr = nullptr;

  return Ok;
}

int mutexattrGettype(ScePthreadMutexattr* attr, int* type) {
  if (attr == nullptr) return getErr(ErrCode::_EINVAL);
  *type = (int)((*attr)->type);
  return Ok;
}

int mutexattrSettype(ScePthreadMutexattr* attr, int type) {
  if (attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  // LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"attr 0x%08llx type:%d", (uint64_t)attr, type);

  (*attr)->type = (SceMutexType)type;
  return Ok;
}

int mutexDestroy(ScePthreadMutex* mutex) {
  if (mutex == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }
  delete *mutex;
  *mutex = nullptr;

  return Ok;
}

auto mutexInit_intern(const ScePthreadMutexattr* attr) {
  auto mutex = std::make_unique<PthreadMutexPrivate>().release();
  if (attr != nullptr) mutex->type = (*attr)->type;
  mutex->id = mutexCounter();
  LOG_USE_MODULE(pthread);
  // LOG_DEBUG(L"mutex ini| id:%llu type:%d", mutex->id, (int)mutex->type);
  return mutex;
}

int mutexInit(ScePthreadMutex* mutex, const ScePthreadMutexattr* attr, const char* name) {

  if (mutex == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *mutex = mutexInit_intern(attr);
  return Ok;
}

static int checkMutexInit(ScePthreadMutex* mutex) {
  if (mutex == nullptr) return getErr(ErrCode::_EINVAL);
  if (*mutex == nullptr) {
    static boost::mutex mutexInt;
    // Is Static Object -> init
    // race condition on first init -> protect
    boost::unique_lock lock(mutexInt);
    if (*mutex == nullptr) *mutex = mutexInit_intern(nullptr);
  }
  return Ok;
}

int mutexLock(ScePthreadMutex* mutex) {
  LOG_USE_MODULE(pthread);
  if (int res = checkMutexInit(mutex); res != Ok) return res;

  // LOG_DEBUG(L"-> mutex lock(%S)| id:%llu thread:%d", (*mutex)->name.c_str(), (*mutex)->id, getThreadId());
  (*mutex)->p.lock();
  int ret = Ok;
  if ((*mutex)->type != SceMutexType::RECURSIVE && (*mutex)->p.recursion_count > 1) {
    ret = getErr(ErrCode::_EDEADLK);
    (*mutex)->p.unlock();
  }
  // LOG_DEBUG(L"<- mutex lock(%S)| id:%llu result:%d", (*mutex)->name.c_str(), (*mutex)->id, ret);

  return ret;
}

int mutexTrylock(ScePthreadMutex* mutex) {
  if (int res = checkMutexInit(mutex); res != Ok) return res;

  int ret = (*mutex)->p.try_lock();
  if (ret > 0) {
    if ((*mutex)->type != SceMutexType::RECURSIVE && (*mutex)->p.recursion_count > 1) {
      ret = getErr(ErrCode::_EDEADLK);
      (*mutex)->p.unlock();
    }
    return Ok;
  }

  return getErr(ErrCode::_EBUSY);
}

int mutexTimedlock(ScePthreadMutex* mutex, SceKernelUseconds usec) {
  LOG_USE_MODULE(pthread);
  if (int res = checkMutexInit(mutex); res != Ok) return res;

  int ret = (*mutex)->p.try_lock_for(boost::chrono::microseconds(usec));
  if (ret > 0) {
    if ((*mutex)->type != SceMutexType::RECURSIVE && (*mutex)->p.recursion_count > 1) {
      ret = getErr(ErrCode::_EDEADLK);
      (*mutex)->p.unlock();
    }
    return Ok;
  }
  return getErr(ErrCode::_ETIMEDOUT);
}

int mutexTimedlock(ScePthreadMutex* mutex, const SceKernelTimespec* t) {
  LOG_USE_MODULE(pthread);
  if (int res = checkMutexInit(mutex); res != Ok) return res;

  boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(35000);
  // LOG_TRACE(L"-> mutex timed lock(%S)| id:%llu", (*mutex)->name.c_str(), (*mutex)->id);

  int ret = (*mutex)->p.try_lock_for(getTimeDuration(t));
  if (ret > 0) {
    if ((*mutex)->type != SceMutexType::RECURSIVE && (*mutex)->p.recursion_count > 1) {
      ret = getErr(ErrCode::_EDEADLK);
      (*mutex)->p.unlock();
    }
    return Ok;
  }
  return getErr(ErrCode::_ETIMEDOUT);

  // LOG_TRACE(L"<- mutex timed lock(%S)| id:%llu result:%d", (*mutex)->name.c_str(), (*mutex)->id, result);

  // LOG_TRACE(L"mutex unlock(%S)| id:%llu result:%d", (*mutex)->name.c_str(), (*mutex)->id, result);
}

int mutexUnlock(ScePthreadMutex* mutex) {

  LOG_USE_MODULE(pthread);
  if (*mutex == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  // LOG_DEBUG(L"-> mutex unlock(%S)| id:%llu thread:%d", (*mutex)->name.c_str(), (*mutex)->id, getThreadId());
  int ret = Ok;
  if ((*mutex)->p.locking_thread_id != boost::winapi::GetCurrentThreadId()) {
    ret = getErr(ErrCode::_EPERM);
  } else {
    (*mutex)->p.unlock();
  }
  // LOG_DEBUG(L"<- mutex unlock(%S)| id:%llu result:%d", (*mutex)->name.c_str(), (*mutex)->id, ret);
  return ret;
}

int rwlockDestroy(ScePthreadRwlock* rwlock) {
  if (rwlock == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  // LOG_INFO(L"<-- rwlock: %S, %d", (*rwlock)->name.c_str(), result);

  delete *rwlock;
  *rwlock = nullptr;

  return Ok;
}

auto createRwLock_intern(const char* name) {
  auto rwlock = std::make_unique<PthreadRwlockPrivate>().release();
  rwlock->id  = mutexCounter();
  if (name != nullptr) rwlock->name = name;
  return rwlock;
}

int rwlockInit(ScePthreadRwlock* rwlock, const ScePthreadRwlockattr* attr, const char* name) {
  if (rwlock == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *rwlock = createRwLock_intern(name);

  // LOG_DEBUG(L"-> rwlock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id, result);
  return Ok;
}

static int checkRwLockInit(ScePthreadRwlock* rwlock) {
  if (*rwlock == nullptr) {
    // Is Static Object -> init
    static boost::mutex mutexInt;
    // Is Static Object -> init
    // race condition on first init -> protect
    boost::unique_lock lock(mutexInt);
    if (*rwlock == nullptr) *rwlock = createRwLock_intern(nullptr);
  }
  return Ok;
}

int rwlockRdlock(ScePthreadRwlock* rwlock) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;
  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);

  try {
    (*rwlock)->p.lock_shared();
  } catch (...) {
    // LOG_TRACE(L"<- rdlock lock(%S)| id:%llu result:EAGAIN", (*rwlock)->name.c_str(), (*rwlock)->id);
    return getErr(ErrCode::_EAGAIN);
  }

  // LOG_TRACE(L"<- rdlock lock(%S)| id:%llu", (*rwlock)->name.c_str(), (*rwlock)->id);
  return Ok;
}

int rwlockTimedrdlock(ScePthreadRwlock* rwlock, SceKernelUseconds usec) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;
  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  auto ret = (*rwlock)->p.timed_lock_shared(boost::chrono::duration(boost::chrono::microseconds(usec))) ? Ok : getErr(ErrCode::_ETIMEDOUT);

  // LOG_TRACE(L"<- rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  return ret;
}

int rwlockTimedrdlock(ScePthreadRwlock* rwlock, const SceKernelTimespec* t) {
  if (t == nullptr) return getErr(ErrCode::_EINVAL);
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;

  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  auto ret = (*rwlock)->p.timed_lock_shared(getTimeDuration(t)) ? Ok : getErr(ErrCode::_ETIMEDOUT);

  // LOG_TRACE(L"<- rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  return ret;
}

int rwlockTimedwrlock(ScePthreadRwlock* rwlock, SceKernelUseconds usec) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;
  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rwlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  auto ret = (*rwlock)->p.timed_lock(boost::chrono::duration(boost::chrono::microseconds(usec))) ? Ok : getErr(ErrCode::_ETIMEDOUT);
  if (ret == Ok) {
    (*rwlock)->isWrite = true;
  }
  // LOG_TRACE(L"<- rwlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  return ret;
}

int rwlockTimedwrlock(ScePthreadRwlock* rwlock, const SceKernelTimespec* t) {
  if (t == nullptr) return getErr(ErrCode::_EINVAL);

  if (int res = checkRwLockInit(rwlock); res != Ok) return res;
  auto const endTime = boost::chrono::high_resolution_clock::now();
  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  auto ret = (*rwlock)->p.timed_lock(getTimeDuration(t)) ? Ok : getErr(ErrCode::_ETIMEDOUT);
  if (ret == Ok) {
    (*rwlock)->isWrite = true;
  }
  // LOG_TRACE(L"<- rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);

  return ret;
}

int rwlockTryrdlock(ScePthreadRwlock* rwlock) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;
  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  auto ret = (*rwlock)->p.try_lock_shared() ? Ok : getErr(ErrCode::_EBUSY);
  // LOG_TRACE(L"<- rdlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  return ret;
}

int rwlockTrywrlock(ScePthreadRwlock* rwlock) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;

  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rwlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  auto ret = (*rwlock)->p.try_lock() ? Ok : getErr(ErrCode::_EBUSY);
  if (ret == Ok) {
    (*rwlock)->isWrite = true;
  }
  // LOG_TRACE(L"-> rwlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  return ret;
}

int rwlockUnlock(ScePthreadRwlock* rwlock) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;
  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rdunlock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);

  if ((*rwlock)->isWrite) {
    (*rwlock)->isWrite = false;
    (*rwlock)->p.unlock();
  } else
    (*rwlock)->p.unlock_shared();
  return Ok;
}

int rwlockWrlock(ScePthreadRwlock* rwlock) {
  if (int res = checkRwLockInit(rwlock); res != Ok) return res;

  LOG_USE_MODULE(pthread);
  // LOG_TRACE(L"-> rwlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);

  (*rwlock)->p.lock();
  (*rwlock)->isWrite = true;
  // LOG_TRACE(L"<- rwlock lock(%S)| id:%llu result:%d", (*rwlock)->name.c_str(), (*rwlock)->id);
  return Ok;
}

int rwlockattrDestroy(ScePthreadRwlockattr* attr) {
  if (attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  delete *attr;
  *attr = nullptr;
  return Ok;
}

int rwlockattrSettype(ScePthreadRwlockattr* attr, int type) {
  if (attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->type = type;

  return Ok;
}

int rwlockattrInit(ScePthreadRwlockattr* attr) {
  *attr = new PthreadRwlockattrPrivate();

  return Ok;
}

int rwlockattrGettype(ScePthreadRwlockattr* attr, int* type) {
  if (type == nullptr || attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *type = (*attr)->type;

  return Ok;
}

int setspecific(ScePthreadKey key, const void* value) {
  accessRuntimeExport()->setTLSKey(key, value);
  return Ok;
}

void* getspecific(ScePthreadKey key) {
  return accessRuntimeExport()->getTLSKey(key);
}

int cancel(ScePthread_obj obj) {
  if (obj == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto thread = getPthread(obj);
  // todo cancel
  // int  result = ::pthread_cancel(thread->p);

  LOG_USE_MODULE(pthread);
  LOG_ERR(L" todo cancel| %S id:%d", thread->name.data(), thread->unique_id);
  // LOG_TRACE(L"thread cancel| threadId:%d name:%S result:%d", thread->unique_id, thread->name.c_str(), result);

  return Ok;
}

int setcancelstate(int state, int* oldState) {
  auto old = g_cancelState.exchange((SceCancelState)state);
  if (oldState != nullptr) *oldState = (int)old;

  return Ok;
}

int setcanceltype(int type, int* oldType) {
  auto old = g_cancelType.exchange((SceCancelType)type);
  if (oldType != nullptr) *oldType = (int)old;

  return Ok;
}

void testCancel(void) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L" test cancel");
  // todo
  //::pthread_testcancel();
}

int getprio(ScePthread_obj obj, int* prio) {
  if (obj == nullptr || prio == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto thread = getPthread(obj);
  *prio       = thread->attr.getShedParam().sched_priority;
  return Ok;
}

int setprio(ScePthread_obj obj, int prio) {
  if (obj == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  auto thread = getPthread(obj);
  thread->attr.setShedParam({.sched_priority = prio});
  setThreadPrio(thread->p.native_handle(), thread->attr.getShedParam().sched_priority);
  return Ok;
}

void yield(void) {
  boost::this_thread::yield();
}

int mutexattrGetprioceiling(ScePthreadMutexattr* attr, int* prio) {
  *prio = (*attr)->prioCeiling;
  return Ok;
}

int mutexattrSetprioceiling(ScePthreadMutexattr* attr, int prio) {
  (*attr)->prioCeiling = prio;
  return Ok;
}

int mutexGetprioceiling(ScePthreadMutex* mutex, int* prio) {
  *prio = (*mutex)->prioCeiling;
  return Ok;
}

int mutexSetprioceiling(ScePthreadMutex* mutex, int prio, int* oldPrio) {
  if (oldPrio != nullptr) *oldPrio = (*mutex)->prioCeiling;
  (*mutex)->prioCeiling = prio;
  return Ok;
}

int mutexattrGetprotocol(ScePthreadMutexattr* attr, int* protocol) {
  *protocol = (int)(*attr)->pprotocol;
  return Ok;
}

int mutexattrSetprotocol(ScePthreadMutexattr* attr, int protocol) {
  (*attr)->pprotocol = (SceMutexProtocol)protocol;
  return Ok;
}

int mutexattrGetpshared(ScePthreadMutexattr* attr, int* pshared) {
  *pshared = (int)(*attr)->pshared;
  return Ok;
}

int mutexattrSetpshared(ScePthreadMutexattr* attr, int pshared) {
  (*attr)->pshared = (SceMutexShared)pshared;
  return Ok;
}

int attrGetinheritsched(const ScePthreadAttr* attr, SceInheritShed* inheritSched) {
  if (inheritSched == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *inheritSched = (*attr)->getInheritShed();
  return Ok;
}

int attrGetschedparam(const ScePthreadAttr* attr, SceSchedParam* param) {
  if (param == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *param = (*attr)->getShedParam();
  return Ok;
}

int attrGetschedpolicy(const ScePthreadAttr* attr, SceShedPolicy* policy) {
  if (policy == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *policy = (*attr)->getPolicy();
  return Ok;
}

int attrSetinheritsched(ScePthreadAttr* attr, SceInheritShed inheritSched) {
  if (attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setInheritShed(inheritSched);
  return Ok;
}

int attrSetschedparam(ScePthreadAttr* attr, const SceSchedParam* param) {
  if (param == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setShedParam(*param);
  return Ok;
}

int attrSetschedpolicy(ScePthreadAttr* attr, SceShedPolicy policy) {
  if (attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setPolicy(policy);
  return Ok;
}

int getschedparam(ScePthread_obj obj, int* policy, SceSchedParam* param) {
  if (obj == nullptr || param == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto thread = getPthread(obj);
  if (policy != nullptr) *policy = thread->policy;
  *param = thread->attr.getShedParam();
  return Ok;
}

int setschedparam(ScePthread_obj obj, int policy, const SceSchedParam* param) {
  if (obj == nullptr || param == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  auto thread = getPthread(obj);

  thread->policy = policy;
  thread->attr.setShedParam(*param);

  return Ok;
}

int attrGetaffinity(const ScePthreadAttr* attr, SceKernelCpumask* mask) {
  if (mask == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *mask = (*attr)->getAffinity();

  return Ok;
}

int attrSetaffinity(ScePthreadAttr* attr, const SceKernelCpumask mask) {
  if (attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  (*attr)->setAffinity(mask);
  return Ok;
}

int getaffinity(ScePthread_obj obj, SceKernelCpumask* mask) {
  if (obj == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  auto thread = getPthread(obj);
  *mask       = thread->attr.getAffinity();
  return Ok;
}

int setaffinity(ScePthread_obj obj, const SceKernelCpumask mask) {
  if (obj == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  auto thread = getPthread(obj);
  int  result = Ok;
  thread->attr.setAffinity(mask);

  return result;
}

ScePthread_obj& getSelf() {
  thread_local ScePthread_obj g_pthread_obj = nullptr;
  return g_pthread_obj;
}

int getthreadid(void) {
  return getPthread(getSelf())->unique_id;
}

int once(ScePthreadOnce once_control, pthread_once_init init_routine) {
  if (once_control == nullptr || init_routine == nullptr) return getErr(ErrCode::_EINVAL);

  auto res = mutexLock(&once_control->mutex);

  if (once_control->isInit == 0) {
    init_routine();
    once_control->isInit = 1;
  }

  res = mutexUnlock(&once_control->mutex);

  return Ok;
}

int rename(ScePthread_obj obj, const char* name) {
  if (obj == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  auto thread  = getPthread(obj);
  thread->name = std::format("_{}_{}", thread->unique_id, name);
  util::setThreadName(thread->name, (void*)thread->p.native_handle());
  return Ok;
}

int getName(ScePthread_obj obj, char* name) {
  if (obj == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  auto thread = getPthread(obj);
  strcpy_s(name, 32, thread->name.c_str());
  return Ok;
}

void setThreadDtors(thread_dtors_func_t dtors) {
  *getThreadDtors() = dtors;
}

auto attrInit_intern() {
  auto attr = std::make_unique<PthreadAttrPrivate>().release();

  return attr;
}

int attrInit(ScePthreadAttr* attr) {
  LOG_USE_MODULE(pthread);
  *attr = attrInit_intern();
  return *attr != nullptr ? Ok : getErr(ErrCode::_ENOMEM);
}

int attrGet(ScePthread_obj obj, ScePthreadAttr* attr) {
  if (obj == nullptr || attr == nullptr || *attr == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  *attr       = attrInit_intern();
  auto thread = getPthread(obj);
  **attr      = thread->attr;
  return Ok;
}

void exit(void* value) {
  LOG_USE_MODULE(pthread);

  auto thread = getPthread(getSelf());

  // todo: unwinding
  thread->arg = value;
  unwinding_longjmp(&thread->_unwinding);
}

void raise(ScePthread_obj obj, void* callback, int signo) {
  auto thread = getPthread(obj);
  QueueUserAPC2((PAPCFUNC)callback, thread->p.native_handle(), (ULONG_PTR)signo, QUEUE_USER_APC_FLAGS_SPECIAL_USER_APC);
}

void* threadWrapper(void* arg);

int create(ScePthread_obj* obj, const ScePthreadAttr* attr, pthread_entry_func_t entry, void* arg, const char* name) {
  // Init pthread once
  {
    static boost::once_flag once;
    boost::call_once(once, init_pThread);
  }
  // -

  LOG_USE_MODULE(pthread);

  if (obj == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }
  std::unique_lock lock(getData()->mutex);

  // Create thread
  auto const tlsStaticSize = accessRuntimeExport()->getTLSStaticBlockSize();
  *obj                     = new uint8_t[sizeof(tlsStaticSize) + tlsStaticSize + sizeof(PthreadPrivate)] {};
  ((uint64_t*)*obj)[0]     = sizeof(tlsStaticSize) + tlsStaticSize; // pthreadOffset

  auto pthread = getPthread(*obj);
  pthread      = new (pthread) PthreadPrivate();

  initTLS(*obj);
  // -

  auto thread = getPthread(*obj);

  if (attr != nullptr) thread->attr = **attr;

  thread->unique_id = threadCounter();

  if (name != nullptr)
    thread->name = std::format("_{}_{}", thread->unique_id, name);
  else
    thread->name = std::format("_{}", thread->unique_id);

  thread->entry    = entry;
  thread->arg      = arg;
  thread->started  = false;
  thread->detached = thread->attr.getDetachState() == SceDetachState::DETACHED;

  boost::thread::attributes boostAttr;
  boostAttr.set_stack_size(thread->attr.getStackSize());
  //!! start_thread_noexcept in boost shouldn't contain STACK_SIZE_PARAM_IS_A_RESERVATION!

  thread->p = boost::thread(boostAttr, boost::bind(threadWrapper, *obj));

  setThreadPrio(thread->p.native_handle(), thread->attr.getShedParam().sched_priority);
  thread->started.wait(true); // race cond with wrapped thread start
  if (thread->attr.getDetachState() == SceDetachState::DETACHED) {
    // thread->p.detach(); // Signaling (raiseSignal) doesnt work when detached
  }
  LOG_INFO(L"--> threadId:%d name:%S addr:0x%08llx stackSize:0x%08llx detached:%d parent:%d", thread->unique_id, thread->name.c_str(), &thread->p,
           thread->attr.getStackSize(), thread->detached, getSelf() != nullptr ? getThreadId() : 0);
  return Ok;
}

int attrSetscope(ScePthreadAttr* attr, int flag) {
  (*attr)->setScope(flag);
  return Ok;
}

void cxa_finalize(void* /*p*/) {
  LOG_USE_MODULE(pthread);
  LOG_ERR(L"pthread_cxa_finalize");
}

void cleanup_push(thread_clean_func_t func, void* arg) {
  auto thread = getPthread(getSelf());
  thread->cleanupFuncs.push_back(std::make_pair(func, arg));
}

void cleanup_pop(int execute) {
  auto thread = getPthread(getSelf());
  if (execute > 0) {
    auto const& [func, arg] = thread->cleanupFuncs.back();
    func(arg);
  }
  thread->cleanupFuncs.pop_back();
}

int attrGetscope(ScePthreadAttr* attr, int* flag) {
  *flag = (*attr)->getScope();
  return Ok;
}

uint8_t* getTLSStaticBlock(ScePthread_obj obj) {
  return (uint8_t*)((uint64_t*)obj)[1]; // 0: size
}

uint64_t* getDTV(ScePthread_obj obj) {
  auto thread = getPthread(obj);
  return thread->dtv;
}

int getThreadId() {
  return getPthread(getSelf())->unique_id;
}

int getThreadId(ScePthread_obj obj) {
  return getPthread(obj)->unique_id;
}

void cleanup_thread() {
  auto thread = getPthread(getSelf());
  LOG_USE_MODULE(pthread);
  LOG_DEBUG(L"cleanup thread:%d", thread->unique_id);

  while (!thread->cleanupFuncs.empty()) {
    auto const& [func, arg] = thread->cleanupFuncs.back();
    func(arg);
    thread->cleanupFuncs.pop_back();
  }

  auto thread_dtors = *getThreadDtors();

  if (thread_dtors != nullptr) {
    thread_dtors();
  }

  accessRuntimeExport()->destroyTLSKeys(getSelf());

  // Delete here if detached, else in join()
  if (thread->detached) {
    LOG_DEBUG(L"Delete thread:%d", thread->unique_id);
    thread->p.detach();
    thread->~PthreadPrivate();

    delete[] getSelf();
  }
}

ScePthread setup_thread(void* arg) {
  auto& gSelf  = getSelf();
  gSelf        = static_cast<ScePthread_obj>(arg);
  auto  thread = getPthread(gSelf);
  auto& attr   = thread->attr;

  LOG_USE_MODULE(pthread);
  // Set Stack Data -> for page_fault allocs
  {
    NT_TIB* tib = getTIB();

    attr.setStackAddr(tib->StackLimit); // lowest addressable byte
    auto const stackSize = (uint64_t)tib->StackBase - (uint64_t)tib->StackLimit;

    LOG_DEBUG(L"thread[%d] stack addr:0x%08llx size:0x%08llx", thread->unique_id, tib->StackBase, stackSize);
    if (attr.getStackSize() < stackSize) {
      LOG_USE_MODULE(pthread);
      LOG_WARN(L"wrong stack size");
    }
    attr.setStackSize(stackSize);
  }
  // -

  boost::this_thread::at_thread_exit(cleanup_thread);
  thread->started = true;
  thread->started.notify_one();
  util::setThreadName(thread->name);

  return thread;
}

__declspec(noinline) SYSV_ABI void* callEntry(ScePthread thread) {
  if (unwinding_setjmp(&thread->_unwinding)) {
    return thread->entry(thread->arg);
  }
  return thread->arg;
}

void* threadWrapper(void* arg) {
  void* ret    = 0;
  auto  thread = setup_thread(arg);
  auto  addr   = &setup_thread;

  int oldType = 0;
  LOG_USE_MODULE(pthread);

  ret = callEntry(thread);

  LOG_DEBUG(L"thread done:%d", thread->unique_id);
  return ret;
}
} // namespace pthread
