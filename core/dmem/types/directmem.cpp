#include "../memoryManager.h"
#include "core/kernel/filesystem.h"
#include "core/runtime/procParam.h"
#include "core/runtime/runtimeLinker.h"
#include "core/videoout/videoout.h"
#include "logging.h"
#include "memory.h"
#include "modules/external/libkernel/codes.h"
#include "modules/external/libkernel/dmem.h"
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
  Reserved,
  Commited,
};

struct MemoryMappingDirect {
  uint64_t addr = 0;

  uint64_t heapAddr = 0; // addr for MemoryInfo

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
  VmaVirtualBlock      vmaBlock = nullptr;

  MemoryState state = MemoryState::Free;
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

class DirectMemory: public IMemoryType {
  mutable boost::mutex m_mutexInt;

  IMemoryManager* m_parent;

  std::map<uint64_t, MemoryInfo>          m_objects;
  std::map<uint64_t, MemoryMappingDirect> m_mappings;

  uint64_t m_allocSize = 0;
  uint64_t m_usedSize  = 0;

  uint64_t m_sdkVersion = 0;

  VmaVirtualBlock m_virtualDeviceMemory;

  uint64_t getSDKVersion() {
    if (m_sdkVersion == 0) {
      auto* procParam = (ProcParam*)accessRuntimeLinker().mainModuleInfo().procParamAddr;
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
  DirectMemory(IMemoryManager* parent): m_parent(parent) {
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

  void setTotalSize(uint64_t totalSize) final;

  int alloc(size_t len, size_t alignment, int memoryType, uint64_t* outAddr) final;
  int free(off_t start, size_t len) final;

  int map(uint64_t vaddr, off_t directMemoryOffset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) final;
  int unmap(uint64_t vaddr, uint64_t size) final;

  virtual int reserve(uint64_t start, size_t len, size_t alignment, int flags, uint64_t* outAddr) final;

  uint64_t size() const final;

  int getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) const final;

  int32_t virtualQuery(uint64_t addr, SceKernelVirtualQueryInfo* info) const final;
  int32_t directQuery(uint64_t offset, SceKernelDirectMemoryQueryInfo* info) const final;

  void deinit() final;
};

std::unique_ptr<IMemoryType> createDirectMemory(IMemoryManager* parent) {
  return std::make_unique<DirectMemory>(parent);
}

void DirectMemory::setTotalSize(uint64_t totalSize) {
  LOG_USE_MODULE(DirectMemory);

  vmaDestroyVirtualBlock(m_virtualDeviceMemory);

  VmaVirtualBlockCreateInfo blockCreateInfo = {
      .size = totalSize,
  };

  if (auto result = vmaCreateVirtualBlock(&blockCreateInfo, &m_virtualDeviceMemory); result != VK_SUCCESS) {
    LOG_CRIT(L"vmaCreateVirtualBlock err:%S", string_VkResult(result));
  }
}

MemoryInfo* DirectMemory::getMemoryInfo(uint64_t len, uint64_t alignment, int prot, VmaVirtualAllocation& outAlloc, uint64_t& outAddr) {
  if (m_objects.empty()) return nullptr;

  VmaVirtualAllocationCreateInfo allocCreateInfo = {
      .size      = len,
      .alignment = alignment,
  };

  for (auto& item: m_objects) {
    if (item.second.state == MemoryState::Reserved) continue;

    if (item.second.state == MemoryState::Commited && prot != item.second.prot) continue;

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

  m_allocSize += len;
  m_objects.emplace(std::make_pair(
      *outAddr, MemoryInfo {.addr = *outAddr, .size = len, .alignment = alignment, .memoryType = memoryType, .vmaAlloc = alloc, .vmaBlock = block}));
  m_parent->registerMapping(*outAddr, len, MappingType::Direct);
  LOG_DEBUG(L"-> Allocated: len:0x%08llx alignment:0x%08llx memorytype:%d -> 0x%08llx", len, alignment, memoryType, *outAddr);

  return Ok;
}

int DirectMemory::free(off_t start, size_t len) {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);

  uint64_t addr = DIRECTMEM_START + start;

  m_allocSize -= len;

  // Find the object
  if (m_objects.empty()) return Ok;

  auto itHeap = m_objects.lower_bound(addr);
  if (itHeap == m_objects.end() || (itHeap != m_objects.begin() && itHeap->first != addr)) --itHeap; // Get the correct item

  if (!(itHeap->first <= addr && (itHeap->first + itHeap->second.size >= addr))) return Ok; // Can't be found
  //-

  LOG_DEBUG(L"free| addr:0x%08llx len:0x%08llx heap -> addr:0x%08llx len:0x%08llx", addr, len, itHeap->first, itHeap->second.size);

  // special: Check reserve if full reservation or commits
  if (itHeap->second.state == MemoryState::Reserved) {
    // todo
    VirtualFree((void*)itHeap->second.addr, itHeap->second.size, 0);
    return Ok;
  }

  if (len != 0 && (itHeap->first != addr || itHeap->second.size != len)) {
    LOG_ERR(L"free Error| start:0x%08llx len:0x%08llx != start:0x%08llx len:0x%08llx", addr, len, itHeap->first, itHeap->second.size);
  }

  if (checkIsGPU(itHeap->second.prot)) {
    // todo

  } else {
    if (itHeap->second.allocAddr != 0) VirtualFree(itHeap->second.allocAddr, itHeap->second.size, 0);
  }

  {
    std::list<uint64_t> dump;
    for (auto& item: m_mappings) {
      if (item.second.heapAddr == itHeap->first) {
        m_usedSize -= item.second.size;
        LOG_TRACE(L"Missing unmap for addr:0x%08llx len:0x%08llx, force unmap", item.second.addr, item.second.size);

        vmaVirtualFree(itHeap->second.vmaBlock, item.second.vmaAlloc);
        m_parent->unregisterMapping(item.first);
        dump.push_back(item.first);
      }

      for (auto item: dump) {
        m_mappings.erase(item);
      }
    }
  }

  vmaDestroyVirtualBlock(itHeap->second.vmaBlock);
  vmaVirtualFree(m_virtualDeviceMemory, itHeap->second.vmaAlloc);

  m_objects.erase(itHeap);

  return Ok;
}

int DirectMemory::map(uint64_t vaddr, off_t offset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);
  if (flags & (int)filesystem::SceMapMode::VOID_) {
    LOG_CRIT(L"todo void map");
  }

  uint64_t desVaddr = 0;
  if (flags & (int)filesystem::SceMapMode::FIXED) {
    desVaddr = vaddr;
  }

  VmaVirtualAllocation alloc = nullptr;

  // search for free space
  uint64_t fakeAddrOffset = 0;

  MemoryInfo* info = nullptr;
  if (flags & (int)filesystem::SceMapMode::FIXED) {
    for (auto& item: m_objects) {
      if (item.second.state == MemoryState::Reserved && item.first <= vaddr && item.second.size >= len) {
        info     = &item.second;
        desVaddr = info->addr;
      }
    }
  }
  if (info == nullptr) info = getMemoryInfo(len, alignment, prot, alloc, fakeAddrOffset);

  if (info == nullptr) return retNoMem();
  // -

  // Check if Commit needed
  if (info->state == MemoryState::Free || info->state == MemoryState::Reserved) {
    MEM_ADDRESS_REQUIREMENTS addressReqs    = {0};
    MEM_EXTENDED_PARAMETER   extendedParams = {0};

    addressReqs.Alignment             = 0; // 64 KB alignment
    addressReqs.LowestStartingAddress = (PVOID)DIRECTMEM_START;
    addressReqs.HighestEndingAddress  = (PVOID)0;

    extendedParams.Type    = MemExtendedParameterAddressRequirements;
    extendedParams.Pointer = &addressReqs;

    uint32_t flags = MEM_COMMIT;
    if (info->state != MemoryState::Reserved) {
      flags |= MEM_RESERVE | MEM_WRITE_WATCH;
    }
    void* ptr = VirtualAlloc2(NULL, (void*)desVaddr, info->size, flags, convProtection(prot), desVaddr != 0 ? 0 : &extendedParams, desVaddr != 0 ? 0 : 1);
    if (ptr == 0) {
      auto const err = GetLastError();
      LOG_ERR(L"Commit Error| addr:0x%08llx len:0x%08llx err:%d", info->addr, info->size, GetLastError());
      return getErr(ErrCode::_EINVAL);
    }

    info->allocAddr = ptr;
    info->state     = MemoryState::Commited;
    info->prot      = prot;
    LOG_DEBUG(L"Commit| addr:0x%08llx len:0x%08llx prot:%d ->", info->addr, info->size, prot, ptr);
  }
  // - commit

  *outAddr = (uint64_t)info->allocAddr + fakeAddrOffset;
  m_mappings.emplace(
      std::make_pair(*outAddr, MemoryMappingDirect {.addr = *outAddr, .heapAddr = info->addr, .size = len, .alignment = alignment, .vmaAlloc = alloc}));

  m_usedSize += len;

  LOG_DEBUG(L"-> Map: start:0x%08llx(0x%x) len:0x%08llx alignment:0x%08llx prot:%d -> 0x%08llx", vaddr, offset, len, alignment, prot, *outAddr);

  if (checkIsGPU(prot)) {
    if (!accessVideoOut().notify_allocHeap(*outAddr, len, prot)) {
      return getErr(ErrCode::_EINVAL);
    }
  }

  return Ok;
}

int DirectMemory::reserve(uint64_t start, size_t len, size_t alignment, int flags, uint64_t* outAddr) {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);

  MEM_ADDRESS_REQUIREMENTS addressReqs    = {0};
  MEM_EXTENDED_PARAMETER   extendedParams = {0};

  addressReqs.Alignment             = alignment;
  addressReqs.LowestStartingAddress = (PVOID)0x100000000;
  addressReqs.HighestEndingAddress  = (PVOID)0;

  extendedParams.Type    = MemExtendedParameterAddressRequirements;
  extendedParams.Pointer = &addressReqs;

  *outAddr = (uint64_t)VirtualAlloc2(NULL, 0, len, MEM_RESERVE | MEM_WRITE_WATCH, PAGE_NOACCESS, &extendedParams, 1);
  if (*outAddr == 0) {
    auto const err = GetLastError();
    LOG_ERR(L"Reserve Error| addr:0x%08llx len:0x%08llx align:0x%08llx flags:0x%x err:%d", start, len, alignment, flags, GetLastError());
    return getErr(ErrCode::_EINVAL);
  }

  LOG_DEBUG(L"-> Reserve: start:0x%08llx len:0x%08llx alignment:0x%08llx flags:0x%x -> 0x%08llx", start, len, alignment, flags, *outAddr);

  m_objects.emplace(
      std::make_pair(*outAddr, MemoryInfo {.addr = *outAddr, .size = len, .alignment = alignment, .memoryType = 0, .state = MemoryState::Reserved}));
  m_parent->registerMapping(*outAddr, len, MappingType::Direct);
  return Ok;
}

int DirectMemory::unmap(uint64_t vaddr, uint64_t size) {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);

  // Find the object
  if (m_mappings.empty()) return Ok;

  auto itItem = m_mappings.lower_bound(vaddr);
  if (itItem == m_mappings.end() || (itItem != m_mappings.begin() && itItem->first != vaddr)) --itItem; // Get the correct item

  if (!(itItem->first <= vaddr && (itItem->first + itItem->second.size >= vaddr))) {
    LOG_ERR(L"Couldn't find mapping for 0x%08llx 0x%08llx", vaddr, size);
    return -1;
  }
  //-

  if (auto it = m_objects.find(itItem->second.heapAddr); it != m_objects.end()) {
    vmaVirtualFree(it->second.vmaBlock, itItem->second.vmaAlloc);
  } else {
    LOG_ERR(L"Couldn't find Heap for 0x%08llx", itItem->second.heapAddr);
  }

  LOG_DEBUG(L"unmap| 0x%08llx 0x%08llx", vaddr, size);
  m_usedSize -= itItem->second.size;

  m_mappings.erase(itItem);
  return Ok;
}

uint64_t DirectMemory::size() const {
  return SCE_KERNEL_MAIN_DMEM_SIZE;
}

int DirectMemory::getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) const {
  LOG_USE_MODULE(DirectMemory);
  LOG_DEBUG(L"availableSize: start:0x%lx end:0x%lx alignment:0x%08llx", start, end, alignment);

  auto itItem = m_objects.lower_bound(DIRECTMEM_START + start);
  if (m_objects.empty() || itItem == m_objects.end()) {
    *startOut = start;
    *sizeOut  = std::min(SCE_KERNEL_MAIN_DMEM_SIZE, (uint64_t)end - start);
    return Ok;
  }

  // *startOut = start;
  // *sizeOut  = (itItem->second.addr - DIRECTMEM_START) - start;
  if (itItem->second.addr + itItem->second.size >= DIRECTMEM_START + end) {
    *startOut = end;
    *sizeOut  = 0;
    return Ok;
  }

  *startOut = start;
  *sizeOut  = 0;

  auto itEnd = m_objects.lower_bound(DIRECTMEM_START + end);
  for (; itItem != itEnd; ++itItem) {
    *startOut = (itItem->second.addr + itItem->second.size) - DIRECTMEM_START;
  }

  if (*startOut > end)
    *sizeOut = *startOut - end;
  else
    *sizeOut = end - *startOut;

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

int32_t DirectMemory::virtualQuery(uint64_t addr, SceKernelVirtualQueryInfo* info) const {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);
  if (m_objects.empty()) return getErr(ErrCode::_EACCES);

  auto itItem = m_objects.lower_bound(addr);
  if (itItem == m_objects.end() && addr > (itItem->first + itItem->second.size)) return getErr(ErrCode::_EACCES); // End reached

  if (itItem == m_objects.end() || (itItem != m_objects.begin() && itItem->first != addr)) --itItem; // Get the correct item

  info->protection       = itItem->second.prot;
  info->memoryType       = itItem->second.memoryType;
  info->isFlexibleMemory = false;
  info->isDirectMemory   = true;
  info->isPooledMemory   = false;
  // info->isStack   = false; // done by parent

  auto itMapping = m_mappings.lower_bound(itItem->first);
  if (itMapping == m_mappings.end() || (itMapping != m_mappings.begin() && itMapping->first != itItem->first)) --itMapping; // Get the correct item

  if (!(itMapping->first <= itItem->first && (itMapping->first + itMapping->second.size > itItem->first))) {
    if (itItem->second.state == MemoryState::Reserved) {
      info->start       = (void*)itItem->first;
      info->end         = (void*)(itItem->first + itItem->second.size);
      info->offset      = 0;
      info->isCommitted = false;
      return Ok;
    }
    return getErr(ErrCode::_EACCES);
  }

  info->start  = (void*)itMapping->first;
  info->end    = (void*)(itMapping->first + itMapping->second.size);
  info->offset = 0;

  info->isCommitted = true;
  return Ok;
}

int32_t DirectMemory::directQuery(uint64_t offset, SceKernelDirectMemoryQueryInfo* info) const {
  LOG_USE_MODULE(DirectMemory);

  boost::unique_lock lock(m_mutexInt);

  for (auto& item: m_objects) {
    auto off = item.second.addr - DIRECTMEM_START;
    if (offset >= off && offset < off + item.second.size) {
      info->start = item.second.addr;
      info->end   = item.second.addr + item.second.size;

      info->memoryType = item.second.memoryType;

      LOG_DEBUG(L"Query OK: offset:0x%08llx -> start:0x%08llx end:0x%08llx type:%d", offset, info->start, info->end, info->memoryType);
      return Ok;
    }
  }

  LOG_DEBUG(L"Query Error: offset:0x%08llx", offset);

  return getErr(ErrCode::_EACCES);
}
