#pragma once

#include "utility/utility.h"

class IMemoryType {
  CLASS_NO_COPY(IMemoryType);
  CLASS_NO_MOVE(IMemoryType);

  protected:
  IMemoryType() {}

  public:
  virtual ~IMemoryType() = default;

  virtual int alloc(size_t len, size_t alignment, int memoryType, uint64_t* outAddr) = 0;
  virtual int free(off_t start, size_t len)                                          = 0;

  virtual int map(uint64_t vaddr, off_t directMemoryOffset, size_t len, int prot, int flags, size_t alignment, uint64_t* outAddr) = 0;
  virtual int unmap(uint64_t vaddr, uint64_t size)                                                                                = 0;

  virtual int reserve(uint64_t start, size_t len, size_t alignment, int flags, uint64_t* outAddr) = 0;

  virtual uint64_t size() const                                                                                          = 0;
  virtual int      getAvailableSize(uint32_t start, uint32_t end, size_t alignment, uint32_t* startOut, size_t* sizeOut) = 0;

  virtual void deinit() = 0;
};