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
LOG_DEFINE_MODULE(FlexibleMemory);

namespace {
constexpr uint64_t DIRECTMEM_START = 0x100000000u;

enum class MemoryState {
  Free,
  Reserved,
  Commited,
};

struct MemoryMapping {
  uint64_t addr = 0;
  uint64_t size = 0;
  int      prot = 0;
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

class FlexibleMemory: public IMemoryType {
  mutable boost::mutex m_mutexInt;

  IMemoryManager* m_parent;

  std::map<uint64_t, MemoryMapping> m_mappings;

  uint64_t m_configuresSize = 448 * 1024 * 1024;

  uint64_t m_usedSize = 0;

  public:
  FlexibleMemory(IMemoryManager* parent): m_parent(parent) {}

  virtual ~FlexibleMemory() { deinit(); }

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

std::unique_ptr<IMemoryType> createFlexibleMemory(IMemoryManager* parent) {
  return std::make_unique<FlexibleMemory>(parent);
}

void FlexibleMemory::setTotalSize(uint64_t totalSize) {
  LOG_USE_MODULE(FlexibleMemory);

  m_configuresSize = totalSize;
}

int FlexibleMemory::alloc(size_t len, size_t alignment, int memoryType, uint64_t* outAddr) {
  LOG_USE_MODULE(FlexibleMemory);

  LOG_CRIT(L"NOT IMPLEMENTED");
  return Ok;
}

int FlexibleMemory::free(off_t start, size_t len) {
  LOG_USE_MODULE(FlexibleMemory);

  LOG_CRIT(L"NOT IMPLEMENTED");
  return Ok;
}

int FlexibleMemory::map(uint64_t vaddr, off_t offset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) {
  LOG_USE_MODULE(FlexibleMemory);

  boost::unique_lock lock(m_mutexInt);
  if (flags & (int)filesystem::SceMapMode::VOID_) {
    LOG_CRIT(L"todo void map");
  }

  uint64_t desVaddr = 0;
  if (flags & (int)filesystem::SceMapMode::FIXED) {
    desVaddr = vaddr;
  }

  if (m_configuresSize <= (m_usedSize + len)) return getErr(ErrCode::_ENOMEM);

  //  Commit
  {
    MEM_ADDRESS_REQUIREMENTS addressReqs    = {0};
    MEM_EXTENDED_PARAMETER   extendedParams = {0};

    addressReqs.Alignment             = 0; // 64 KB alignment
    addressReqs.LowestStartingAddress = (PVOID)DIRECTMEM_START;
    addressReqs.HighestEndingAddress  = (PVOID)0;

    extendedParams.Type    = MemExtendedParameterAddressRequirements;
    extendedParams.Pointer = &addressReqs;

    *outAddr = (uint64_t)VirtualAlloc2(NULL, (void*)desVaddr, len, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, convProtection(prot),
                                       desVaddr != 0 ? 0 : &extendedParams, desVaddr != 0 ? 0 : 1);
    if (*outAddr == 0) {
      auto const err = GetLastError();
      LOG_ERR(L"Commit Error| addr:0x%08llx len:0x%08llx err:%d", desVaddr, len, GetLastError());
      return getErr(ErrCode::_EINVAL);
    }
  }
  // - commit

  m_mappings.emplace(std::make_pair(*outAddr, MemoryMapping {.addr = *outAddr, .size = len, .prot = prot}));

  m_parent->registerMapping(*outAddr, len, MappingType::Flexible);
  m_usedSize += len;

  LOG_DEBUG(L"-> Map: start:0x%08llx(0x%x) len:0x%08llx alignment:0x%08llx prot:%d -> 0x%08llx", vaddr, offset, len, alignment, prot, *outAddr);

  if (checkIsGPU(prot)) {
    if (!accessVideoOut().notify_allocHeap(*outAddr, len, prot)) {
      return getErr(ErrCode::_EINVAL);
    }
  }

  return Ok;
}

int FlexibleMemory::reserve(uint64_t start, size_t len, size_t alignment, int flags, uint64_t* outAddr) {
  LOG_USE_MODULE(FlexibleMemory);

  LOG_CRIT(L"NOT IMPLEMENTED");
  return Ok;
}

int FlexibleMemory::unmap(uint64_t vaddr, uint64_t size) {
  LOG_USE_MODULE(FlexibleMemory);

  boost::unique_lock lock(m_mutexInt);

  // Find the object
  if (m_mappings.empty()) return getErr(ErrCode::_EINVAL);

  auto itItem = m_mappings.lower_bound(vaddr);
  if (itItem == m_mappings.end() || (itItem != m_mappings.begin() && itItem->first != vaddr)) --itItem; // Get the correct item

  if (!(itItem->first <= vaddr && (itItem->first + itItem->second.size >= vaddr))) {
    LOG_ERR(L"Couldn't find mapping for 0x%08llx 0x%08llx", vaddr, size);
    return -1;
  }
  //-

  if (checkIsGPU(itItem->second.prot)) {
    // todo

  } else {
    if (itItem->second.addr != 0) VirtualFree((void*)itItem->second.addr, itItem->second.size, 0);
  }

  LOG_DEBUG(L"unmap| 0x%08llx 0x%08llx", vaddr, size);
  m_usedSize -= itItem->second.size;

  m_mappings.erase(itItem);
  return Ok;
}

uint64_t FlexibleMemory::size() const {
  return m_configuresSize;
}

int FlexibleMemory::getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) const {
  LOG_USE_MODULE(FlexibleMemory);

  *sizeOut = m_configuresSize - m_usedSize;
  LOG_DEBUG(L"availableSize: 0x%08llx ", *sizeOut);

  return Ok;
}

void FlexibleMemory::deinit() {
  for (auto& item: m_mappings) {
    if (checkIsGPU(item.second.prot)) {
      // done by gpuMemoryManager
    } else {
      // memory::free(item.first);
    }
  }
  m_mappings.clear();
}

int32_t FlexibleMemory::virtualQuery(uint64_t addr, SceKernelVirtualQueryInfo* info) const {
  boost::unique_lock lock(m_mutexInt);

  auto itMapping = m_mappings.lower_bound(addr);
  if (itMapping == m_mappings.end() || (itMapping != m_mappings.begin() && itMapping->first != addr)) --itMapping; // Get the correct item

  if (!(itMapping->first <= addr && (itMapping->first + itMapping->second.size >= addr))) {
    return getErr(ErrCode::_EACCES);
  }

  info->protection       = itMapping->second.prot;
  info->memoryType       = 3;
  info->isFlexibleMemory = true;
  info->isDirectMemory   = false;
  info->isPooledMemory   = false;
  // info->isStack   = false; // done by parent

  info->start  = (void*)itMapping->first;
  info->end    = (void*)(itMapping->first + itMapping->second.size);
  info->offset = 0;

  info->isCommitted = true;
  return Ok;
}

int32_t FlexibleMemory::directQuery(uint64_t offset, SceKernelDirectMemoryQueryInfo* info) const {
  LOG_USE_MODULE(FlexibleMemory);
  LOG_CRIT(L"NOT IMPLEMENTED");
  return Ok;
}