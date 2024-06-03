#pragma once

#include "imemory.h"
#include "utility/utility.h"

#include <stdint.h>

enum class MappingType { None, File, Flexible, Fixed, Direct };

struct SceKernelVirtualQueryInfo;

class IMemoryManager {
  CLASS_NO_COPY(IMemoryManager);
  CLASS_NO_MOVE(IMemoryManager);

  public:
  IMemoryManager() = default;

  virtual ~IMemoryManager() = default;

  /**
   * @brief Register mapped memory
   *
   * @param vaddr
   * @param type
   */
  virtual void registerMapping(uint64_t vaddr, uint64_t size, MappingType type) = 0;

  /**
   * @brief Unregisters mapping and returns the type of the mapping
   *
   * @param vaddr
   * @return None: Mapping didn't exist
   */
  virtual MappingType unregisterMapping(uint64_t vaddr) = 0;

  virtual void registerStack(uint64_t addr, uint64_t size) = 0;
  virtual void unregisterStack(uint64_t addr)              = 0;

  virtual int32_t virtualQuery(uint64_t addr, SceKernelVirtualQueryInfo* info) const = 0;

  virtual IMemoryType* directMemory()   = 0;
  virtual IMemoryType* flexibleMemory() = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL IMemoryManager* accessMemoryManager();
#undef __APICALL
