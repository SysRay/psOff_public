#include "common.h"
#include "logging.h"
#include "types.h"

#include <boost/pool/simple_segregated_storage.hpp>
#include <memory>
#include <windows.h>

LOG_DEFINE_MODULE(mspace);

// todo use vmaVirtualAllocate for this!

namespace {
struct MSpaceData {
  boost::simple_segregated_storage<std::size_t> storage;
};

using MSpaceData_t = MSpaceData*;
} // namespace

extern "C" {
EXPORT SYSV_ABI MSpaceData_t sceLibcMspaceCreate(const char* name, void* base, size_t capacity, uint32_t flag) {
  LOG_USE_MODULE(mspace);

  auto* mspace = std::make_unique<MSpaceData>().release();
  LOG_DEBUG(L"+ mspace name:%S ptr:0x%08llx base:0x%08llx size:0x%08llx", name, (uint64_t)mspace, (uint64_t)base, capacity);
  mspace->storage.add_block(base, capacity, 32);

  return mspace;
}

EXPORT SYSV_ABI int sceLibcMspaceDestroy(MSpaceData_t mspace) {
  if (*(uintptr_t*)mspace == 0) {
    // todo internal freeing?
    return Ok;
    // -
  }
  delete mspace;
  return Ok;
}

EXPORT SYSV_ABI int sceLibcMspaceFree(MSpaceData_t mspace, void* block) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    ::free(block);
    return Ok;
    // -
  }

  mspace->storage.free(block);
  return Ok;
}

EXPORT SYSV_ABI void* sceLibcMspaceMalloc(MSpaceData_t mspace, size_t size) {
  LOG_USE_MODULE(mspace);
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    return ::malloc(size);
    // -
  }

  auto addr = mspace->storage.malloc_n(1 + (size / 32), 32);
  LOG_DEBUG(L"mspace ptr:0x%08llx alloc size:0x%08llx @0x%08llx", (uint64_t)mspace, size, addr);
  return addr;
}

EXPORT SYSV_ABI int sceLibcMspaceIsHeapEmpty(MSpaceData_t mspace) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    HANDLE hHeap = GetProcessHeap();
    return ::HeapSize(hHeap, 0, nullptr);
    // -
  }

  LOG_USE_MODULE(mspace);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok; // todo
}

EXPORT SYSV_ABI void* sceLibcMspaceMemalign(MSpaceData_t mspace, uint64_t alignment, uint64_t size) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    return ::_aligned_malloc(size, alignment);
    // -
  }

  LOG_USE_MODULE(mspace);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return (void*)((uint64_t)mspace->storage.malloc_n(1, alignment + size) & ~(alignment - 1));
}

EXPORT SYSV_ABI void* sceLibcMspaceCalloc(MSpaceData_t mspace, uint64_t n, uint64_t size) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    return ::calloc(n, size);
    // -
  }

  return mspace->storage.malloc_n(n, size);
}

EXPORT SYSV_ABI void* sceLibcMspaceReallocalign(MSpaceData_t mspace, void* src, uint64_t alignment, uint64_t size) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    return ::_aligned_realloc(src, alignment, size);
    // -
  }
  LOG_USE_MODULE(mspace);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return nullptr;
}

EXPORT SYSV_ABI void* sceLibcMspaceRealloc(MSpaceData_t mspace, void* src, uint64_t size) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    return ::realloc(src, size);
    // -
  }

  auto dst = mspace->storage.malloc_n(1, size);
  if (dst == nullptr) return 0;

  if (src != nullptr) {
    memcpy(dst, src, size);
    mspace->storage.free(src);
  }
  return dst;
}

EXPORT SYSV_ABI int sceLibcMspacePosixMemalign(MSpaceData_t mspace, void** ptr, uint64_t alignment, uint64_t size) {
  if (*(uintptr_t*)mspace == 0) {
    // internal mspace
    *ptr = ::_aligned_malloc(size, alignment);
    return *ptr == nullptr ? 12 : Ok;
    // -
  }
  LOG_USE_MODULE(mspace);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 12;
}

EXPORT SYSV_ABI int sceLibcMspaceMallocStats(MSpaceData_t mspace) {
  return Ok;
}

EXPORT SYSV_ABI int sceLibcMspaceMallocStatsFast(MSpaceData_t mspace) {
  return Ok;
}

EXPORT SYSV_ABI size_t sceLibcMspaceMallocUsableSize(void* ptr) {
  return 100000; // todo
}
}