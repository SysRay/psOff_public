#pragma once

#include "utility/utility.h"

enum class GpuMemoryMode { NoAccess, Read, Write, ReadWrite };

enum class MappingType { None, File, Flexible, Fixed, Direct };

class IDirectMemory {
  CLASS_NO_COPY(IDirectMemory);
  CLASS_NO_MOVE(IDirectMemory);

  protected:
  IDirectMemory() = default;

  public:
  virtual ~IDirectMemory() = default;

  virtual int alloc(size_t len, size_t alignment, int memoryType, uint64_t* outAddr) = 0;
  virtual int free(off_t start, size_t len)                                          = 0;

  virtual int map(uint64_t vaddr, off_t directMemoryOffset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) = 0;
  virtual int unmap(uint64_t vaddr, uint64_t size)                                                                                = 0;

  virtual int reserve(uint64_t start, size_t len, size_t alignment, int flags, uint64_t* outAddr) = 0;

  virtual uint64_t size() const                                                                                          = 0;
  virtual int      getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) = 0;

  virtual void deinit() = 0;
};

class IFlexibleMemory {
  CLASS_NO_COPY(IFlexibleMemory);

  uint64_t m_configuresSize = 448 * 1024 * 1024;

  protected:
  IFlexibleMemory() = default;

  public:
  virtual ~IFlexibleMemory() = default;

  void setConfiguredSize(uint64_t size) { m_configuresSize = size; }

  uint64_t size() const { return m_configuresSize; }

  virtual uint64_t available()                                 = 0;
  virtual uint64_t alloc(uint64_t vaddr, size_t len, int prot) = 0;
  virtual bool     destroy(uint64_t vaddr, uint64_t size)      = 0;

  virtual void release(uint64_t start, size_t len) = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

/**
 * @brief registers mapping (for mmap, mumap)
 *
 * @param vaddr
 * @param type != None
 * @return __APICALL
 */
__APICALL void registerMapping(uint64_t vaddr, MappingType type);

/**
 * @brief Unregisters mapping and returns the type of the mappin
 *
 * @param vaddr
 * @return None: Mapping didn't exist
 */
__APICALL MappingType unregisterMapping(uint64_t vaddr);

__APICALL IDirectMemory& accessDirectMemory();

__APICALL IFlexibleMemory& accessFlexibleMemory();
#undef __APICALL