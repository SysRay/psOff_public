#include "dmem.h"

#include "assert.h"
#include "common.h"
#include "core/dmem/memoryManager.h"
#include "core/kernel/filesystem.h"
#include "core/memory/memory.h"
#include "internal/videoout/videoout.h"
#include "logging.h"
#include "modules_include/common.h"
#include "types.h"
LOG_DEFINE_MODULE(dmem);

namespace {} // namespace

extern "C" {

EXPORT SYSV_ABI size_t sceKernelGetDirectMemorySize() {
  LOG_USE_MODULE(dmem);

  auto size = accessMemoryManager()->directMemory()->size();
  LOG_DEBUG(L"%S size:0x%08llx", __FUNCTION__, size);
  return size;
}

EXPORT SYSV_ABI int32_t sceKernelMapNamedFlexibleMemory(uint64_t* addrInOut, size_t len, int prot, int flags, const char* name) {
  return accessMemoryManager()->flexibleMemory()->map(*addrInOut, 0, len, prot, flags, 0, addrInOut);
}

EXPORT SYSV_ABI int32_t sceKernelMapFlexibleMemory(uint64_t* addrInOut, size_t len, int prot, int flags) {
  return sceKernelMapNamedFlexibleMemory(addrInOut, len, prot, flags, "");
}

EXPORT SYSV_ABI int32_t sceKernelReleaseFlexibleMemory(uint64_t addr, size_t len) {
  return accessMemoryManager()->flexibleMemory()->unmap(addr, len);
}

EXPORT SYSV_ABI int32_t sceKernelSetPrtAperture(int index, void* addr, size_t len) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelGetPrtAperture(int index, void** addr, size_t* len) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelAllocateDirectMemory(uint64_t searchStart, uint64_t searchEnd, size_t len, size_t alignment, int memoryType,
                                                      uint64_t* physAddrOut) {
  if (len == 0 || physAddrOut == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  return accessMemoryManager()->directMemory()->alloc(len, alignment, memoryType, physAddrOut);
}

EXPORT SYSV_ABI int32_t sceKernelAllocateMainDirectMemory(size_t len, size_t alignment, int memoryType, uint64_t* physAddrOut) {
  if (len == 0 || physAddrOut == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  return accessMemoryManager()->directMemory()->alloc(len, alignment, memoryType, physAddrOut);
}

EXPORT SYSV_ABI int32_t sceKernelReleaseDirectMemory(off_t start, size_t len) {
  return accessMemoryManager()->directMemory()->free(start, len);
}

EXPORT SYSV_ABI int32_t sceKernelCheckedReleaseDirectMemory(off_t start, size_t len) {
  return accessMemoryManager()->directMemory()->free(start, len);
}

EXPORT SYSV_ABI int32_t sceKernelMapNamedDirectMemory(uint64_t* addr, size_t len, int prot, int flags, off_t offset, size_t alignment, const char* name) {
  return accessMemoryManager()->directMemory()->map(*addr, offset, len, prot, flags, alignment, addr);
}

EXPORT SYSV_ABI int32_t sceKernelMapDirectMemory(uint64_t* addr, size_t len, int prot, int flags, off_t offset, size_t maxPageSize) {
  return sceKernelMapNamedDirectMemory(addr, len, prot, flags, offset, maxPageSize, nullptr);
}

EXPORT SYSV_ABI int32_t sceKernelMapDirectMemory2(uint64_t* addr, size_t len, int type, int prot, int flags, off_t offset, size_t maxPageSize) {
  return sceKernelMapNamedDirectMemory(addr, len, prot, flags, offset, maxPageSize, nullptr);
}

EXPORT SYSV_ABI int32_t sceKernelGetDirectMemoryType(off_t start, int* memoryType, off_t* regionStartOut, off_t* regionEndOut) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelBatchMap2(SceKernelBatchMapEntry* entries, int numberOfEntries, int* numberOfEntriesOut, int flags) {
  LOG_USE_MODULE(dmem);
  for (*numberOfEntriesOut = 0; *numberOfEntriesOut < numberOfEntries; ++*numberOfEntriesOut) {
    auto& batchEntry = (entries)[*numberOfEntriesOut];

    switch (batchEntry.operation) {
      case SceKernelMapOp::MAP_DIRECT: {
        auto res =
            accessMemoryManager()->directMemory()->map(batchEntry.start, batchEntry.physAddr, batchEntry.length, batchEntry.prot, flags, 0, &batchEntry.start);
        if (res != Ok) {
          return res;
        }
      } break;
      case SceKernelMapOp::UNMAP: {
        LOG_ERR(L"todo %S op:%d", __FUNCTION__, batchEntry.operation);
      } break;
      case SceKernelMapOp::PROTECT: {
        LOG_ERR(L"todo %S op:%d", __FUNCTION__, batchEntry.operation);
      } break;
      case SceKernelMapOp::MAP_FLEXIBLE: {
        LOG_ERR(L"todo %S op:%d", __FUNCTION__, batchEntry.operation);
      } break;
      case SceKernelMapOp::TYPE_PROTECT: {
        LOG_ERR(L"todo %S op:%d", __FUNCTION__, batchEntry.operation);
      } break;
    }
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelBatchMap(SceKernelBatchMapEntry* items, int size, int* count) {
  return sceKernelBatchMap2(items, size, count, (int)filesystem::SceMapMode::NO_OVERWRITE);
}

EXPORT SYSV_ABI int32_t sceKernelJitCreateSharedMemory(const char* name, size_t len, int maxProt, int* fdOut) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelJitCreateAliasOfSharedMemory(int fd, int maxProt, int* fdOut) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelJitMapSharedMemory(int fd, int prot, void** startOut) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelJitGetSharedMemoryInfo(int fd, char* name, int nameBufferSize, void** startOut, size_t* lenOut, int* maxProtOut) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelQueryMemoryProtection(uintptr_t addr, uintptr_t* start, uintptr_t* end, int* prot) {
  LOG_USE_MODULE(dmem);

  uint64_t base = memory::queryAlloc(addr, start, end, prot);
  if (base == 0) {
    LOG_TRACE(L"query KERNEL_ERROR_EACCES: 0x%08llx", (uint64_t)addr);
    return getErr(ErrCode::_EACCES);
  }

  if (prot != nullptr) {
    if (accessVideoOut().isGPULocal(base)) *prot |= 0x30; // GPU read write
    LOG_TRACE(L"query: 0x%08llx prot:%x", (uint64_t)addr, *prot);
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelIsStack(void* addr, void** start, void** end) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelVirtualQuery(uintptr_t addr, int flags, SceKernelVirtualQueryInfo* info, size_t infoSize) {
  if (info == nullptr || infoSize != sizeof(SceKernelVirtualQueryInfo)) return getErr(ErrCode::_EFAULT);

  LOG_USE_MODULE(dmem);
  constexpr uint64_t DIRECTMEM_START = 0x100000000u;

  if (addr >= DIRECTMEM_START) {
    if (accessMemoryManager()->virtualQuery((uint64_t)addr, info) != Ok) {
      LOG_TRACE(L"Query Error: addr:0x%08llx -> start:0x%08llx, end:0x%08llx, type:%d", addr, info->start, info->end, info->memoryType);
      return getErr(ErrCode::_EACCES);
    }
  } else {
    uint64_t base = (uint64_t)addr;

    if ((uint64_t)addr < IMAGE_BASE) {
      addr = IMAGE_BASE;
    }

    base = memory::queryAlloc(addr, (uintptr_t*)&info->start, (uintptr_t*)&info->end, &info->protection);
    if (base <= 0) {
      if (accessMemoryManager()->virtualQuery((uint64_t)addr, info) != Ok) {
        LOG_TRACE(L"Query Error: addr:0x%08llx -> start:0x%08llx, end:0x%08llx, type:%d", addr, info->start, info->end, info->memoryType);
        return getErr(ErrCode::_EACCES);
      }
    } else {
      info->isCommitted      = true;
      info->isFlexibleMemory = true;
      info->offset           = 0;
    }
  }

  LOG_TRACE(L"Query OK: addr:0x%08llx -> start:0x%08llx end:0x%08llx prot:%d type:%d", addr, info->start, info->end, info->protection, info->memoryType);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelSetVirtualRangeName(void* start, size_t len, const char* name) {
  LOG_USE_MODULE(dmem);
  LOG_TRACE(L"vrange, start:0x%08llx len:0x%08llx name:%S", start, len, name);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelReserveVirtualRange(uintptr_t* addr, size_t len, int flags, size_t alignment) {
  if (addr == nullptr || len == 0) {
    return getErr(ErrCode::_EINVAL);
  }

  return accessMemoryManager()->directMemory()->reserve(*addr, len, alignment, flags, addr);
}

EXPORT SYSV_ABI int32_t sceKernelDirectMemoryQuery(uint64_t offset, int flags, SceKernelDirectMemoryQueryInfo* queryInfo, size_t infoSize) {

  LOG_USE_MODULE(dmem);

  if (offset < 0 || infoSize != sizeof(SceKernelDirectMemoryQueryInfo) || queryInfo == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  return accessMemoryManager()->directMemory()->directQuery(offset, queryInfo);
}

EXPORT SYSV_ABI int32_t sceKernelMtypeprotect(const void* addr, size_t size, int type, int prot) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelAvailableDirectMemorySize(off_t start, off_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) {
  accessMemoryManager()->directMemory()->getAvailableSize(start, end, alignment, startOut, sizeOut);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelGetPageTableStats(int* cpuTotal, int* cpuAvailable, int* gpuTotal, int* gpuAvailable) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolBatch(const SceKernelMemoryPoolBatchEntry* entries, int n, int* indexOut, int flags) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolCommit(uint64_t addr, size_t len, int type, int prot, int flags) {
  LOG_USE_MODULE(dmem);

  uint64_t   outAddr = 0;
  auto const isGpu   = (prot & 0xF0) > 0;
  if (isGpu & ((prot & 0xF) > 0)) {
    outAddr = memory::allocGPUMemory(addr, 0, len, 0);
  } else {
    outAddr = memory::commit(addr, 0, len, 0, prot);
  }
  assert(addr == outAddr);
  LOG_DEBUG(L"poolCommit| addr:0x%08llx len:0x%08llx type:%d prot:%d flag:%d @0x%08llx", addr, len, type, prot, flags, outAddr);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolDecommit(void* addr, size_t len, int flags) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolExpand(off_t searchStart, off_t searchEnd, int64_t len, size_t alignment, off_t* physAddrOut) {
  LOG_USE_MODULE(dmem);

  if (len < 0) return Ok;

  *physAddrOut = memory::reserve(0, len, alignment, false);
  LOG_DEBUG(L"PoolReserve| start:0x%08llx, len:0x%08llx  align:0x%08llx-> out:0x%08llx", searchStart, len, alignment, *physAddrOut);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolGetBlockStats(SceKernelMemoryPoolBlockStats* output, size_t outputSize) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolMove(void* dst, void* src, size_t len, int flags) {
  LOG_USE_MODULE(dmem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelMemoryPoolReserve(uint64_t addrIn, uint64_t len, uint64_t alignment, int flags, uint64_t* addrOut) {
  LOG_USE_MODULE(dmem);

  *addrOut = memory::reserve(addrIn, len, alignment, false);
  LOG_DEBUG(L"PoolReserve| start:0x%08llx, len:0x%08llx  align:0x%08llx flags:%d -> out:0x%08llx", addrIn, len, alignment, flags, *addrOut);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceKernelAvailableFlexibleMemorySize(size_t* sizeOut) {
  return accessMemoryManager()->flexibleMemory()->getAvailableSize(0, 0, 0, 0, sizeOut);
}

EXPORT SYSV_ABI int32_t sceKernelConfiguredFlexibleMemorySize(size_t* sizeOut) {
  *sizeOut = accessMemoryManager()->flexibleMemory()->size();
  return Ok;
}

} // namespace dmem
