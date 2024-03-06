#pragma once
#include <stdint.h>

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

namespace gpuMemory {
__APICALL bool notify_allocHeap(uint64_t vaddr, uint64_t size, int memoryProtection);
__APICALL bool isGPULocal(uint64_t vaddr);
} // namespace gpuMemory

#undef __APICALL