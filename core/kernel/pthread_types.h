#pragma once
#include "utility/utility.h"

using pthread_entry_func_t          = SYSV_ABI void* (*)(void*);
using pthread_key_destructor_func_t = SYSV_ABI void (*)(void*);

using thread_dtors_func_t = SYSV_ABI void (*)();
using thread_clean_func_t = SYSV_ABI void (*)(void*);

using pthread_once_init = SYSV_ABI void (*)();

struct SceSchedParam {
  int sched_priority;
};

enum class SceMutexProtocol { PRIO_NONE, PRIO_INHERIT, PRIO_PROTECT };
enum class SceMutexType { DEFAULT, ERRORCHECK, RECURSIVE, NORMAL, ADAPTIVE_NP };
enum class SceMutexShared { PRIVATE, SHARED };

enum class SceDetachState {
  JOINABLE,
  DETACHED,
};

enum class SceShedPolicy {
  OTHER,
  FIFO,
  RR,
};
enum class SceInheritShed {
  EXPLICIT,
  INHERIT = 4,
};

enum class SceCancelState {
  ENABLE,
  DISABLE,
};
enum class SceCancelType {
  DEFERRED,
  ASYNC = 4,
};

struct SceCleanInfo {
  SceCleanInfo*       prev;
  thread_clean_func_t func;
  void*               arg;
  int                 onHeap;
};
