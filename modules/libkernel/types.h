#pragma once
#include "codes.h"
#include "utility/utility.h"

using SceKernelModule = int32_t;

using get_thread_atexit_count_func_t = SYSV_ABI int (*)(SceKernelModule);
using thread_atexit_report_func_t    = SYSV_ABI void (*)(SceKernelModule);

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