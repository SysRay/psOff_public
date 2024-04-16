#define __APICALL_EXTERN
#include "dmem.h"
#undef __APICALL_EXTERN

#include "core/imports/exports/procParam.h"
#include "core/imports/imports_runtime.h"
#include "core/kernel/filesystem.h"
#include "core/videoout/videoout.h"
#include "logging.h"
#include "modules/libkernel/codes.h"
#include "modules/libkernel/dmem.h"
#include "utility/utility.h"

#include <boost/thread.hpp>
#include <map>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <windows.h>

#undef min
LOG_DEFINE_MODULE(DirectMemory);

namespace {
constexpr uint64_t DIRECTMEM_START = 0x100000000u;

enum class MemoryState {
  Free,
  Commited,
};

struct MemoryMapping {
  uint64_t addr      = 0;
  uint64_t size      = 0;
  uint64_t alignment = 0;

  VmaVirtualAllocation vmaAlloc;
};

struct MemoryInfo {
  uint64_t addr      = 0;
  void*    allocAddr = nullptr;

  uint64_t size      = 0;
  uint64_t alignment = 0;

  int memoryType = 0;
  int prot       = 0;

  VmaVirtualAllocation vmaAlloc;
  VmaVirtualBlock      vmaBlock;

  MemoryState state = MemoryState::Free;

  std::map<uint64_t, MemoryMapping> mappings;
};

bool checkIsGPU(int protection) {
  return (protection & 0xf0) > 0;
}

DWORD convProtection(int prot) {
  switch (prot & 0xf) {
    case 0: return PAGE_NOACCESS;
    case 1: return PAGE_READONLY;
    case 2:
    case 3: return PAGE_READWRITE;
    case 4: return PAGE_EXECUTE;
    case 5: return PAGE_EXECUTE_READ;
    case 6:
    case 7: return PAGE_EXECUTE_READWRITE;
  }

  if (checkIsGPU(prot)) {
    return PAGE_READWRITE; // special case: cpu read/writes gpumemory on host memory
  }

  return PAGE_NOACCESS;
}
} // namespace

class DirectMemory: public IDirectMemory {
  boost::mutex m_mutexInt;

  std::map<uint64_t, MemoryInfo> m_objects;

  uint64_t m_curSize = 0;

  uint64_t m_sdkVersion = 0;

  VmaVirtualBlock m_virtualDeviceMemory;

  uint64_t getSDKVersion() {
    if (m_sdkVersion == 0) {
      auto* procParam = (ProcParam*)accessRuntimeExport()->mainModuleInfo().procParamAddr;
      m_sdkVersion    = procParam->header.sdkVersion;
    }

    return m_sdkVersion;
  }

  int retNoMem() {
    if (m_sdkVersion < 0x3500000) return getErr(ErrCode::_EINVAL);
    return getErr(ErrCode::_ENOMEM);
  }

  MemoryInfo* getMemoryInfo(uint64_t len, uint64_t alignment, int prot, VmaVirtualAllocation& outAlloc, uint64_t& outAddr);

  public:
  DirectMemory() {
    LOG_USE_MODULE(DirectMemory);
    VmaVirtualBlockCreateInfo blockCreateInfo = {
        .size = SCE_KERNEL_MAIN_DMEM_SIZE,
    };

    if (auto result = vmaCreateVirtualBlock(&blockCreateInfo, &m_virtualDeviceMemory); result != VK_SUCCESS) {
      LOG_CRIT(L"vmaCreateVirtualBlock err:%S", string_VkResult(result));
    }
  }

  virtual ~DirectMemory() { deinit(); }

  // ### Interface
  int alloc(size_t len, size_t alignment, int memoryType, uint64_t* outAddr) final;
  int free(off_t start, size_t len) final;

  int  map(uint64_t vaddr, off_t directMemoryOffset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) final;
  bool unmap(uint64_t vaddr, uint64_t size) final;

  uint64_t size() const final;

  int getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) final;

  void deinit() final;
};

IDirectMemory& accessDirectMemory() {
  static DirectMemory obj;
  return obj;
}

MemoryInfo* DirectMemory::getMemoryInfo(uint64_t len, uint64_t alignment, int prot, VmaVirtualAllocation& outAlloc, uint64_t& outAddr) {
  if (m_objects.empty()) return nullptr;

  VmaVirtualAllocationCreateInfo allocCreateInfo = {
      .size      = len,
      .alignment = alignment,
  };

  for (auto& item: m_objects) {
    if (item.second.state != MemoryState::Free && prot != item.second.prot) continue;

    if (auto result = vmaVirtualAllocate(item.second.vmaBlock, &allocCreateInfo, &outAlloc, &outAddr); result == VK_SUCCESS) {
      return &item.second;
    }
  }
  return nullptr;
}

int DirectMemory::alloc(size_t len, size_t alignment, int memoryType, uint64_t* outAddr) {
  LOG_USE_MODULE(DirectMemory);

  if ((alignment > 0 && !util::isPowerOfTwo(alignment)) || (len % SCE_KERNEL_PAGE_SIZE != 0) || (alignment % SCE_KERNEL_PAGE_SIZE != 0))
    return getErr(ErrCode::_EINVAL);

  boost::unique_lock lock(m_mutexInt);

  VmaVirtualAllocationCreateInfo allocCreateInfo = {
      .size      = len,
      .alignment = alignment,
  };

  // Get block from device memory
  VmaVirtualAllocation alloc;

  if (auto result = vmaVirtualAllocate(m_virtualDeviceMemory, &allocCreateInfo, &alloc, outAddr); result != VK_SUCCESS) {
    LOG_ERR(L"Alloc Error| len:0x%08llx alignment:0x%08llx memorytype:%d err:%d", len, alignment, memoryType, GetLastError());
    return getErr(ErrCode::_ENOMEM);
  }
  // - check devicememory

  *outAddr += DIRECTMEM_START; // For debugging info

  // Device has space -> Create allocation
  VmaVirtualBlockCreateInfo blockCreateInfo = {
      .size = len,
  };
  VmaVirtualBlock block;

  if (auto result = vmaCreateVirtualBlock(&blockCreateInfo, &block); result != VK_SUCCESS) {
    LOG_ERR(L"Alloc Error| len:0x%08llx alignment:0x%08llx memorytype:%d err:%d", len, alignment, memoryType, GetLastError());
    return getErr(ErrCode::_ENOMEM);
  }
  // -

  m_curSize += len;
  m_objects.emplace(std::make_pair(
      *outAddr, MemoryInfo {.addr = *outAddr, .size = len, .alignment = alignment, .memoryType = memoryType, .vmaAlloc = alloc, .vmaBlock = block}));

  LOG_DEBUG(L"-> Allocated: len:0x%08llx alignment:0x%08llx memorytype:%d -> 0x%08llx", len, alignment, memoryType, *outAddr);

  return Ok;
}

int DirectMemory::free(off_t start, size_t len) {
  LOG_USE_MODULE(DirectMemory);

  uint64_t addr = DIRECTMEM_START + start;
  LOG_ERR(L"free| addr:0x%08llx len:0x%08llx", addr, len);

  // Find the object
  if (m_objects.empty()) return Ok;

  auto itHeap = m_objects.lower_bound(addr);
  if (itHeap == m_objects.end() || (itHeap != m_objects.begin() && itHeap->first != addr)) --itHeap; // Get the correct item

  //-
  if (!(itHeap->first <= addr && (itHeap->first + itHeap->second.size >= addr))) return Ok; // Can't be found

  if (itHeap->first != addr || itHeap->second.size != len) {
    LOG_ERR(L"free Error| start:0x%08llx len:0x%08llx != start:0x%08llx len:0x%08llx", addr, len, itHeap->first, itHeap->second.size);
  }

  // todo free
  printf("found\n");
  return Ok;
}

int DirectMemory::map(uint64_t vaddr, off_t offset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);
  if (flags & (int)filesystem::SceMapMode::VOID_) {
    LOG_CRIT(L"todo void map");
  }

  VmaVirtualAllocation alloc = nullptr;

  // search for free space
  uint64_t    fakeAddr = 0;
  MemoryInfo* info     = getMemoryInfo(len, alignment, prot, alloc, fakeAddr);
  if (info == nullptr) return retNoMem();
  // -

  // Check if Commit needed
  if (info->state == MemoryState::Free) {
    MEM_ADDRESS_REQUIREMENTS addressReqs    = {0};
    MEM_EXTENDED_PARAMETER   extendedParams = {0};

    addressReqs.Alignment             = 0; // 64 KB alignment
    addressReqs.LowestStartingAddress = (PVOID)0x100000000;
    addressReqs.HighestEndingAddress  = (PVOID)0;

    extendedParams.Type    = MemExtendedParameterAddressRequirements;
    extendedParams.Pointer = &addressReqs;

    void* ptr = VirtualAlloc2(NULL, 0, info->size, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, convProtection(prot), &extendedParams, 1);
    if (ptr == 0) {
      auto const err = GetLastError();
      LOG_ERR(L"Commit Error| addr:0x%08llx len:0x%08llx err:%d", info->addr, info->size, GetLastError());
      return getErr(ErrCode::_EINVAL);
    }

    info->allocAddr = ptr;
    info->state     = MemoryState::Commited;
    LOG_DEBUG(L"Commit| addr:0x%08llx len:0x%08llx prot:%d ->", info->addr, info->size, prot, ptr);
  }
  // - commit

  *outAddr = (uint64_t)info->allocAddr + fakeAddr;
  info->mappings.emplace(std::make_pair(*outAddr, MemoryMapping {.addr = *outAddr, .size = len, .alignment = alignment, .vmaAlloc = alloc}));

  LOG_DEBUG(L"-> Map: start:0x%08llx(0x%lx) len:0x%08llx alignment:0x%08llx prot:%d -> 0x%08llx", vaddr, offset, len, alignment, prot, *outAddr);

  if (checkIsGPU(prot)) {
    if (!accessVideoOut().notify_allocHeap(*outAddr, len, prot)) {
      return getErr(ErrCode::_EINVAL);
    }
  }

  return Ok;
}

bool DirectMemory::unmap(uint64_t vaddr, uint64_t size) {}

uint64_t DirectMemory::size() const {
  return m_curSize;
}

int DirectMemory::getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) {
  LOG_USE_MODULE(DirectMemory);
  LOG_ERR(L"availableSize: start:0x%08llx end:0x%08llx alignment:0x%08llx", start, end, alignment);

  *startOut = size();
  *sizeOut  = SCE_KERNEL_MAIN_DMEM_SIZE - size();

  return Ok;
}

void DirectMemory::deinit() {
  for (auto& item: m_objects) {
    if (checkIsGPU(item.second.prot)) {
      // done by gpuMemoryManager
    } else {
      // memory::free(item.first);
    }
  }
  m_objects.clear();
}