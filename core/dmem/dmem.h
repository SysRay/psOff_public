#pragma once

#include "utility/utility.h"

enum class GpuMemoryMode { NoAccess, Read, Write, ReadWrite };

class IPysicalMemory {
  CLASS_NO_COPY(IPysicalMemory);

  protected:
  IPysicalMemory()         = default;
  uint64_t m_availableSize = 5000000000llu; // todo get from system memory
  size_t   m_allocSize     = 0;

  public:
  void getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) {
    *startOut = size() - m_availableSize;
    *sizeOut  = m_availableSize;
  }

  virtual uint64_t alloc(uint64_t vaddr, size_t len, int memoryType) = 0;

  virtual bool      reserve(uint64_t start, size_t len, size_t alignment, uint64_t* outAddr, int memoryType) = 0;
  virtual uintptr_t commit(uint64_t base, uint64_t offset, size_t len, size_t alignment, int prot)           = 0;

  virtual bool Map(uint64_t vaddr, uint64_t physAddr, size_t len, int prot, bool allocFixed, size_t alignment, uint64_t* outAddr) = 0;
  virtual bool Release(uint64_t start, size_t len, uint64_t* vaddr, uint64_t* size)                                               = 0;
  virtual bool Unmap(uint64_t vaddr, uint64_t size)                                                                               = 0;

  uint64_t const size() const { return m_allocSize; } // use system ram
};

class IFlexibleMemory {
  CLASS_NO_COPY(IFlexibleMemory);

  uint64_t m_configuresSize = 448 * 1024 * 1024;

  protected:
  IFlexibleMemory() = default;

  public:
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

__APICALL IPysicalMemory&  accessPysicalMemory();
__APICALL IFlexibleMemory& accessFlexibleMemory();
#undef __APICALL