#pragma once
#include <stdint.h>

constexpr int SceProtRead    = 1;
constexpr int SceProtWrite   = 2;
constexpr int SceProtExecute = 4;

constexpr uint64_t DIRECTMEM_START = 0x100000000u;

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

namespace memory {
constexpr inline bool isExecute(int prot) {
  return (prot & 0x4) > 0;
}

__APICALL int       getpagesize(void);
__APICALL uint64_t  getTotalSystemMemory();
__APICALL uintptr_t reserve(uint64_t start, uint64_t size, uint64_t alignment, bool isGpu);
__APICALL uint64_t  commit(uintptr_t baseAddr, uint64_t offset, uint64_t size, uint64_t alignment, int prot);
__APICALL uint64_t  allocGPUMemory(uintptr_t baseAddr, uint64_t offset, uint64_t size, uint64_t alignment);
__APICALL int64_t   queryAlloc(uintptr_t addr, uintptr_t* start, uintptr_t* end, int* prot);
__APICALL uint64_t  alloc(uint64_t address, uint64_t size, int prot);
__APICALL uint64_t  allocAligned(uint64_t address, uint64_t size, int prot, uint64_t alignment);
__APICALL bool      allocFixed(uint64_t address, uint64_t size, int prot);
__APICALL bool      free(uint64_t address);
__APICALL bool      protect(uint64_t address, uint64_t size, int prot, int* oldMode = nullptr);
__APICALL int       getProtection(uint64_t address);
} // namespace memory

#undef __APICALL