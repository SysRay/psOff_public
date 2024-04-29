#pragma once

#include "utility/utility.h"

enum class GpuMemoryMode { NoAccess, Read, Write, ReadWrite };

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

__APICALL IFlexibleMemory& accessFlexibleMemory();
#undef __APICALL