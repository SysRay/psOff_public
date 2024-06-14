#define __APICALL_EXTERN
#include "memoryManager.h"
#undef __APICALL_EXTERN

#include "logging.h"
#include "modules/external/libkernel/dmem.h"
#include "types/memory.h"

#include <boost/thread.hpp>
#include <map>

LOG_DEFINE_MODULE(MemoryManager);

namespace {
struct MappingData {
  MappingType type;
  uint64_t    size;
};
} // namespace

class MemoryManager: public IMemoryManager {

  std::unique_ptr<IMemoryType> m_directMemory;
  std::unique_ptr<IMemoryType> m_flexibleMemory;

  mutable boost::mutex m_mutexInt;

  std::map<uint64_t, MappingData> m_mappings;

  std::map<uint64_t, uint64_t> m_stackAddrMap;

  public:
  MemoryManager() {
    m_directMemory   = createDirectMemory(this);
    m_flexibleMemory = createFlexibleMemory(this);
  }

  ~MemoryManager() = default;

  // ### Interface
  void        registerMapping(uint64_t vaddr, uint64_t size, MappingType type) final;
  MappingType unregisterMapping(uint64_t vaddr) final;

  void registerStack(uint64_t addr, uint64_t size) final;
  void unregisterStack(uint64_t addr) final;

  int32_t virtualQuery(uint64_t addr, SceKernelVirtualQueryInfo* info) const final;

  IMemoryType* directMemory() final { return m_directMemory.get(); }

  IMemoryType* flexibleMemory() final { return m_flexibleMemory.get(); }
};

IMemoryManager* accessMemoryManager() {
  static MemoryManager inst;
  return &inst;
}

void MemoryManager::registerMapping(uint64_t vaddr, uint64_t size, MappingType type) {

  boost::unique_lock lock(m_mutexInt);

  m_mappings.emplace(std::make_pair(vaddr, MappingData {.type = type, .size = size}));
}

MappingType MemoryManager::unregisterMapping(uint64_t vaddr) {
  boost::unique_lock lock(m_mutexInt);

  if (auto it = m_mappings.find((uint64_t)vaddr); it != m_mappings.end()) {
    auto type = it->second.type;
    m_mappings.erase(it);
    return type;
  }

  return MappingType::None;
}

void MemoryManager::registerStack(uint64_t addr, uint64_t size) {
  boost::unique_lock lock(m_mutexInt);
  m_stackAddrMap[addr] = size;
}

void MemoryManager::unregisterStack(uint64_t addr) {
  boost::unique_lock lock(m_mutexInt);
  m_stackAddrMap.erase(addr);
}

int32_t MemoryManager::virtualQuery(uint64_t addr, SceKernelVirtualQueryInfo* info) const {
  LOG_USE_MODULE(MemoryManager);

  boost::unique_lock lock(m_mutexInt);

  auto itItem = m_mappings.lower_bound(addr);

  if (itItem == m_mappings.end() && addr > (itItem->first + itItem->second.size)) return getErr(ErrCode::_EACCES); // End reached

  if (itItem == m_mappings.end() || (itItem != m_mappings.begin() && itItem->first != addr)) --itItem; // Get the correct item

  if (itItem->first + itItem->second.size <= addr) return getErr(ErrCode::_EACCES); // Is last item

  int res = getErr(ErrCode::_EACCES);

  switch (itItem->second.type) {
    case MappingType::Direct: {
      res = m_directMemory->virtualQuery(itItem->first, info);
    } break;
    case MappingType::Flexible: {
      res = m_flexibleMemory->virtualQuery(itItem->first, info);
    } break;
    case MappingType::Fixed: {
    } break;
    case MappingType::File: {
    } break;

    default: break;
  }

  if (res == Ok) {
    LOG_TRACE(L"Query OK: addr:0x%08llx - start:0x%08llx end:0x%08llx prot:%d type:%d", itItem->first, info->start, info->end, info->protection,
              info->memoryType);
  } else {
    LOG_TRACE(L"Query Error: addr:0x%08llx", addr);
  }

  return res;
};