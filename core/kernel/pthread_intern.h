#pragma once
#include "core/unwinding/unwind.h"
#include "modules_include/common.h"
#include "pthread_types.h"
#include "utility/utility.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>

constexpr size_t   DEFAULT_STACKSIZE = 16 * 1024 * 1024;
constexpr uint64_t XSAVE_CHK_GUARD   = 0xDeadBeef5533CCAAu;

constexpr int KEYS_MAX              = 256;
constexpr int DESTRUCTOR_ITERATIONS = 4;

constexpr size_t DTV_MAX_KEYS = 256;

struct PthreadAttrPrivate {
  private:
  size_t stackSize = DEFAULT_STACKSIZE;
  void*  stackAddr = 0; /// if != 0 use the custum stack

  SceSchedParam    shedParam   = {.sched_priority = 700};
  SceKernelCpumask affinity    = 0x7f;
  size_t           guardSize   = 0x1000;
  SceShedPolicy    policy      = SceShedPolicy::FIFO;
  int              scope       = 0;
  SceDetachState   detachState = SceDetachState::JOINABLE;

  SceInheritShed inheritShed = SceInheritShed::INHERIT;

  public:
  PthreadAttrPrivate() = default;

  auto getStackAddr() const noexcept { return stackAddr; }

  void setStackAddr(decltype(PthreadAttrPrivate::stackAddr) param) noexcept { stackAddr = param; }

  auto getStackSize() const noexcept { return stackSize; }

  void setStackSize(decltype(PthreadAttrPrivate::stackSize) param) noexcept { stackSize = param; }

  auto getGuardSize() const noexcept { return guardSize; }

  void setGuardSize(decltype(PthreadAttrPrivate::guardSize) param) noexcept { guardSize = param; }

  auto getAffinity() const noexcept { return affinity; }

  void setAffinity(decltype(PthreadAttrPrivate::affinity) param) noexcept { affinity = param; }

  auto getPolicy() const noexcept { return policy; }

  void setPolicy(decltype(PthreadAttrPrivate::policy) param) noexcept { policy = param; }

  auto getScope() const noexcept { return scope; }

  void setScope(decltype(PthreadAttrPrivate::scope) param) noexcept { scope = param; }

  auto getShedParam() const noexcept { return shedParam; }

  void setShedParam(decltype(PthreadAttrPrivate::shedParam) param) noexcept { shedParam = param; }

  auto getDetachState() const noexcept { return detachState; }

  void setDetachState(decltype(PthreadAttrPrivate::detachState) param) noexcept { detachState = param; }

  auto getInheritShed() const noexcept { return inheritShed; }

  void setInheritShed(decltype(PthreadAttrPrivate::inheritShed) param) noexcept { inheritShed = param; }
};

struct PthreadMutexPrivate {
  uint64_t               initialized = 1;
  uint8_t                reserved[256]; // type is bigger on ps than here
  boost::recursive_mutex p;
  std::string            name;
  size_t                 id;
  int                    prioCeiling;
  SceMutexType           type = SceMutexType::DEFAULT;

  ~PthreadMutexPrivate() {}
};

struct PthreadMutexattrPrivate {
  uint64_t initialized = 1;
  uint8_t  reserved[60]; // type is bigger on ps than here

  SceMutexShared   pshared   = SceMutexShared::PRIVATE;
  SceMutexType     type      = SceMutexType::DEFAULT;
  SceMutexProtocol pprotocol = SceMutexProtocol::PRIO_NONE;
  int              prioCeiling;
};

struct DTVKey {
  bool used = false;

  pthread_key_destructor_func_t destructor = nullptr;
};

constexpr size_t DTV_SIZE = DTV_MAX_KEYS;

struct PthreadPrivate {

  uint64_t    dtv[DTV_SIZE];
  std::string name;

  boost::thread p;

  PthreadAttrPrivate   attr      = {};
  pthread_entry_func_t entry     = nullptr;
  void*                arg       = nullptr;
  int                  unique_id = 0;
  std::atomic_bool     started   = false;
  int                  policy    = 0;

  std::vector<std::pair<thread_clean_func_t, void*>> cleanupFuncs;

  bool detached = false; // fake detach

  unwinding_jmp_buf _unwinding;
  ~PthreadPrivate() = default;
};

struct PthreadRwlockPrivate {
  uint64_t initialized = 1;
  uint8_t  reserved[256]; // type is bigger on ps than here

  boost::shared_mutex p;
  std::string         name;
  size_t              id;

  bool isWrite = false;

  ~PthreadRwlockPrivate() {}
};

struct PthreadBarrierPrivate {};

struct PthreadBarrierattrPrivate {
  int pshared;
};

struct PthreadCondattrPrivate {
  uint64_t initialized = 1;
  uint8_t  reserved[64]; // type is bigger on ps than here
};

struct PthreadRwlockattrPrivate {
  uint64_t initialized = 1;
  uint8_t  reserved[64]; // type is bigger on ps than here
  int      type;
};

struct PthreadCondPrivate {
  uint64_t initialized = 1;
  uint8_t  reserved[64]; // type is bigger on ps than here

  boost::condition_variable p;
  std::string               name;

  ~PthreadCondPrivate() {}
};

struct PthreadOnce {
  int32_t isInit; // 0: no, 1: yes
  int32_t _align;

  PthreadMutexPrivate* mutex;
};
