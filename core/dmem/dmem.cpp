#define __APICALL_EXTERN
#include "dmem.h"
#undef __APICALL_EXTERN

#include "core/memory/memory.h"
#include "core/videoout/videoout.h"
#include "logging.h"
#include "memoryManager.h"
#include "utility/utility.h"

#include <algorithm>
#include <boost/thread.hpp>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

LOG_DEFINE_MODULE(MemoryManager);

namespace {

struct MemoryInfo {
  bool   isGpu = false;
  size_t size  = 0;
};

struct pImpl {
  boost::mutex mutex_;

  std::unordered_map<uint64_t, MappingType> mappings_;
};

auto getData() {
  static pImpl obj;
  return &obj;
}
} // namespace

class FlexibleMemory: public IFlexibleMemory {
  uint64_t   m_totalAllocated = 0;
  std::mutex m_mutex_int;

  public:
  FlexibleMemory()          = default;
  virtual ~FlexibleMemory() = default;

  uint64_t alloc(uint64_t vaddr, size_t len, int prot) final;
  bool     destroy(uint64_t vaddr, uint64_t size) final;

  void release(uint64_t start, size_t len) final;

  uint64_t available() final {
    std::unique_lock const lock(m_mutex_int);
    return (uint64_t)448 * 1024 * 1024 - m_totalAllocated; // todo get system ram
  }
};

IFlexibleMemory& accessFlexibleMemory() {
  static FlexibleMemory inst;
  return inst;
}

uint64_t FlexibleMemory::alloc(uint64_t vaddr, size_t len, int prot) {
  LOG_USE_MODULE(MemoryManager);

  std::unique_lock const lock(m_mutex_int);
  m_totalAllocated += len;

  auto const outAddr = memory::alloc(vaddr, len, prot);

  accessMemoryManager()->registerMapping(outAddr, MappingType::Flexible);
  LOG_INFO(L"--> Heap| vaddr:0x%08llx len:%llu prot:0x%x total:0x%08llx -> @0x%08llx", vaddr, len, prot, m_totalAllocated, outAddr);
  return outAddr;
}

bool FlexibleMemory::destroy(uint64_t vaddr, uint64_t size) {
  LOG_USE_MODULE(MemoryManager);

  std::unique_lock const lock(m_mutex_int);
  m_totalAllocated -= size;

  memory::free(vaddr);
  LOG_INFO(L"<-- Heap| vaddr:0x%08llx len:%lld total:0x%08llx", vaddr, size, m_totalAllocated);

  return true;
}

void FlexibleMemory::release(uint64_t start, size_t len) {
  LOG_USE_MODULE(MemoryManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

void registerMapping(uint64_t vaddr, MappingType type) {
  auto impl = getData();

  boost::unique_lock lock(impl->mutex_);

  impl->mappings_.emplace(std::make_pair(vaddr, type));
}

MappingType unregisterMapping(uint64_t vaddr) {
  auto impl = getData();

  boost::unique_lock lock(impl->mutex_);

  if (auto it = impl->mappings_.find((uint64_t)vaddr); it != impl->mappings_.end()) {
    auto type = it->second;
    impl->mappings_.erase(it);
    return it->second;
  }

  return MappingType::None;
}