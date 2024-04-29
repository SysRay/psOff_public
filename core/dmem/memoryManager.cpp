#define __APICALL_EXTERN
#include "memoryManager.h"
#undef __APICALL_EXTERN

#include "logging.h"
#include "types/memory.h"

#include <boost/thread.hpp>
#include <unordered_map>

LOG_DEFINE_MODULE(MemoryManager);

class MemoryManager: public IMemoryManager {

  std::unique_ptr<IMemoryType> m_directMemory;
  std::unique_ptr<IMemoryType> m_flexibleMemory;

  boost::mutex m_mutexInt;

  std::unordered_map<uint64_t, MappingType> m_mappings;

  public:
  MemoryManager() { m_directMemory = createDirectMemory(this); }

  ~MemoryManager() = default;

  // ### Interface
  void        registerMapping(uint64_t vaddr, MappingType type) final;
  MappingType unregisterMapping(uint64_t vaddr) final;

  IMemoryType* directMemory() final { return m_directMemory.get(); }
};

IMemoryManager* accessMemoryManager() {
  static MemoryManager inst;
  return &inst;
}

void MemoryManager::registerMapping(uint64_t vaddr, MappingType type) {

  boost::unique_lock lock(m_mutexInt);

  m_mappings.emplace(std::make_pair(vaddr, type));
}

MappingType MemoryManager::unregisterMapping(uint64_t vaddr) {
  boost::unique_lock lock(m_mutexInt);

  if (auto it = m_mappings.find((uint64_t)vaddr); it != m_mappings.end()) {
    auto type = it->second;
    m_mappings.erase(it);
    return it->second;
  }

  return MappingType::None;
}