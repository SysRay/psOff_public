#pragma once
#include "utility/utility.h"

using pthread_entry_func_t          = SYSV_ABI void* (*)(void*);
using pthread_key_destructor_func_t = SYSV_ABI void (*)(void*);

struct SceSchedParam {
  int sched_priority;
};

enum class SceMutexProtocol { PRIO_NONE, PRIO_INHERIT, PRIO_PROTECT };
enum class SceMutexType { DEFAULT, ERRORCHECK, RECURSIVE, NORMAL, ADAPTIVE_NP };

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
