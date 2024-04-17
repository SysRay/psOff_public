#pragma once
#include <modules_include/common.h>

enum class SceKernelMemoryType { WB_ONION = 0, WC_GARLIC = 3, WB_GARLIC, MEMORY_TYPE_END };
enum class SceKernelMapOp : int {
  MAP_DIRECT,
  UNMAP,
  PROTECT,
  MAP_FLEXIBLE,
  TYPE_PROTECT,
};

struct SceKernelBatchMapEntry {
  uint64_t       start;
  uint32_t       physAddr;
  size_t         length;
  uint8_t        prot;
  uint8_t        type;
  short          pad1;
  SceKernelMapOp operation;
};

struct SceKernelVirtualQueryInfo {
  void*    start;
  void*    end;
  int64_t  offset;
  int      protection;
  int      memoryType;
  unsigned isFlexibleMemory : 1;
  unsigned isDirectMemory   : 1;
  unsigned isStack          : 1;
  unsigned isPooledMemory   : 1;
  unsigned isCommitted      : 1;
  char     name[32];
};

struct SceKernelDirectMemoryQueryInfo {
  int64_t start;
  int64_t end;
  int     memoryType;
};

struct SceKernelMemoryPoolBatchEntry {
  unsigned op;
  unsigned flags;

  union {
    struct {
      void*         addr;
      size_t        len;
      unsigned char prot;
      unsigned char type;
    } commit;

    struct {
      void*  addr;
      size_t len;
    } decommit;

    struct {
      void*         addr;
      size_t        len;
      unsigned char prot;
    } protect;

    struct {
      void*         addr;
      size_t        len;
      unsigned char prot;
      unsigned char type;
    } typeProtect;

    struct {
      void*  dst;
      void*  src;
      size_t len;
    } move;

    uintptr_t padding[3];
  };
};

struct SceKernelMemoryPoolBlockStats {
  int availableFlushedBlocks;
  int availableCachedBlocks;
  int allocatedFlushedBlocks;
  int allocatedCachedBlocks;
};
