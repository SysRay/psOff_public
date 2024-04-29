#pragma once
#include <stdint.h>

struct SceKernelMemParam {
  uint64_t*       sceKernelExtendedPageTable;
  uint64_t*       sceKernelFlexibleMemorySize;
  uint8_t*        sceKernelExtendedMemory1;
  uint64_t const* sceKernelExtendedGpuPageTable;
  uint8_t const*  sceKernelExtendedMemory2;
  uint64_t const* sceKernelExtendedCpuPageTable;
};

struct SceKernelFsParam {
  uint64_t  size;
  uint64_t* sceKernelFsDupDent;
  void*     sceWorkspaceUpdateMode;
  void*     sceTraceAprNameMode;
};

struct SceMallocReplace {
  uint64_t size               = sizeof(SceMallocReplace);
  uint64_t unknown1           = 1;
  void*    malloc_init        = nullptr;
  void*    malloc_finalize    = nullptr;
  void*    malloc             = nullptr;
  void*    free               = nullptr;
  void*    calloc             = nullptr;
  void*    realloc            = nullptr;
  void*    memalign           = nullptr;
  void*    reallocalign       = nullptr;
  void*    posix_memalign     = nullptr;
  void*    malloc_stats       = nullptr;
  void*    malloc_stats_fast  = nullptr;
  void*    malloc_usable_size = nullptr;
};

struct SceLibcNewReplace {
  uint64_t Size                      = sizeof(SceLibcNewReplace);
  uint64_t Unknown1                  = 0x2;
  void*    user_new                  = nullptr;
  void*    user_new_nothrow          = nullptr;
  void*    user_new_array            = nullptr;
  void*    user_new_array_nothrow    = nullptr;
  void*    user_delete               = nullptr;
  void*    user_delete_nothrow       = nullptr;
  void*    user_delete_array         = nullptr;
  void*    user_delete_array_nothrow = nullptr;
  // optional
  // void* user_delete_3= nullptr;
  // void* user_delete_4= nullptr;
  // void* user_delete_array_3= nullptr;
  // void* user_delete_array_4= nullptr;
};

struct SceLibcParam0 {
  uint64_t Size;
  uint32_t entry_count;
};

struct SceLibcParam1: SceLibcParam0 {
  uint32_t           SceLibcInternalHeap; //(entry_count > 1)
  uint32_t*          sceLibcHeapSize;
  uint32_t*          sceLibcHeapDelayedAlloc;
  uint32_t*          sceLibcHeapExtendedAlloc;
  uint32_t*          sceLibcHeapInitialSize;
  SceMallocReplace*  _sceLibcMallocReplace;
  SceLibcNewReplace* _sceLibcNewReplace;
};

struct SceLibcParam2: SceLibcParam1 {
  uint64_t* sceLibcHeapHighAddressAlloc; //(entry_count > 2)
  uint32_t* Need_sceLibc;
};

struct SceLibcParam3: SceLibcParam2 {
  uint64_t* sceLibcHeapMemoryLock; //(entry_count > 4)
  uint64_t* sceKernelInternalMemorySize;
  uint64_t* _sceLibcMallocReplaceForTls;
};

struct SceLibcParam4: SceLibcParam3 {
  uint64_t* sceLibcMaxSystemSize; //(entry_count > 7)
};

struct SceLibcParam5: SceLibcParam4 {
  uint64_t* sceLibcHeapDebugFlags; //(entry_count > 8)
  uint32_t* sceLibcStdThreadStackSize;
  void*     Unknown3;
  uint32_t* sceKernelInternalMemoryDebugFlags;
  uint32_t* sceLibcWorkerThreadNum;
  uint32_t* sceLibcWorkerThreadPriority;
  uint32_t* sceLibcThreadUnnamedObjects;
};

struct ProcParam {
  struct {
    uint64_t size;       /// 0x50
    uint32_t magic;      /// "orbi" (0x4942524F)
    uint32_t entryCount; /// >=1
    uint64_t sdkVersion; /// 0x4508101
  } header;

  char const*        sceProcessName;
  char const*        sceUserMainThreadName;
  uint32_t const*    sceUserMainThreadPriority;
  uint32_t const*    sceUserMainThreadStackSize;
  SceLibcParam0*     PSceLibcParam;
  SceKernelMemParam* _sceKernelMemParam;
  SceKernelFsParam*  _sceKernelFsParam;
  uint32_t*          sceProcessPreloadEnabled;
  uint64_t*          unknown;
};