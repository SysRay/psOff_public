#pragma once
#include "codes.h"
#include "modules_include/common.h"
#include "utility/utility.h"

using SceKernelModule = int32_t;

using get_thread_atexit_count_func_t = SYSV_ABI int (*)(SceKernelModule);
using thread_atexit_report_func_t    = SYSV_ABI void   (*)(SceKernelModule);

typedef int   SceKernelAioSubmitId;
typedef void* sigset_t;

enum class SceKernelCPUMode : int {
  CPUMODE_6CPU        = 0,
  CPUMODE_7CPU_LOW    = 1,
  CPUMODE_7CPU_NORMAL = 5,
};

struct SceKernelLoadModuleOpt {
  size_t size;
};

struct SceKernelUnloadModuleOpt {
  size_t size;
};

struct TlsInfo {
  uint64_t index;
  uint64_t offset;
};

struct SceModuleUndwindInfo {
  uint64_t size;
  char     name[255];
  void*    eh_frame_hdr_addr;
  void*    eh_frame_addr;
  uint64_t eh_frame_size;
  uint64_t seg0Addr; // Main vaddr (normaly offset:0)
  uint64_t seg0Size;
};

struct SceKernelAioSchedulingParam {
  int      schedulingWindowSize;
  int      delayedCountLimit;
  uint32_t enableSplit;
  uint32_t splitSize;
  uint32_t splitChunkSize;
};

struct SceKernelAioParam {
  SceKernelAioSchedulingParam low;
  SceKernelAioSchedulingParam mid;
  SceKernelAioSchedulingParam high;
};

struct SceKernelAioResult {
  int64_t  returnValue;
  uint32_t state;
};

struct SceKernelAioRWRequest {
  int64_t             offset;
  size_t              nbyte;
  void*               buf;
  SceKernelAioResult* result;
  int                 fd;
};

enum class rusageWho : int {
  Children = -1,
  Self     = 0,
  Thread   = 1,
};

struct rusage_t {
  SceKernelTimeval ru_utime    = {0, 0};
  SceKernelTimeval ru_stime    = {0, 0};
  uint32_t         ru_maxrss   = 0;
  uint32_t         ru_ixrss    = 0;
  uint32_t         ru_idrss    = 0;
  uint32_t         ru_isrss    = 0;
  uint32_t         ru_minflt   = 0;
  uint32_t         ru_majflt   = 0;
  uint32_t         ru_nswap    = 0;
  uint32_t         ru_inblock  = 0;
  uint32_t         ru_oublock  = 0;
  uint32_t         ru_msgsnd   = 0;
  uint32_t         ru_msgrcv   = 0;
  uint32_t         ru_nsignals = 0;
  uint32_t         ru_nvcsw    = 0;
  uint32_t         ru_nivcsw   = 0;
};

struct timesec {
  time_t        t;
  unsigned long westsec;
  unsigned long dstsec;
};
