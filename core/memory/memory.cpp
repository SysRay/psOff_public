#define __APICALL_EXTERN
#include "memory.h"
#undef __APICALL_EXTERN

#include "logging.h"
#include "utility/utility.h"

#include <windows.h>

LOG_DEFINE_MODULE(memory);

namespace {
bool checkIsGPU(int protection) {
  return (protection & 0xf0) > 0;
}

DWORD convProtection(int prot) {
  switch (prot & 0xf) {
    case 0: return PAGE_NOACCESS;
    case 1: return PAGE_READONLY;
    case 2:
    case 3: return PAGE_READWRITE;
    case 4: return PAGE_EXECUTE;
    case 5: return PAGE_EXECUTE_READ;
    case 6:
    case 7: return PAGE_EXECUTE_READWRITE;
  }

  if (checkIsGPU(prot)) {
    return PAGE_READWRITE; // special case: cpu read/writes gpumemory on host memory
  }

  return PAGE_NOACCESS;
}

int convProtection(DWORD prot) {
  switch (prot) {
    case PAGE_NOACCESS: return 0;
    case PAGE_READONLY: return SceProtRead;
    case PAGE_READWRITE: return SceProtRead | SceProtWrite;
    case PAGE_EXECUTE: return SceProtExecute;
    case PAGE_EXECUTE_READ: return SceProtExecute | SceProtRead;
    case PAGE_EXECUTE_READWRITE: return SceProtRead | SceProtWrite | SceProtExecute;
  }

  return 0;
}

using VirtualAlloc2_func_t = /*WINBASEAPI*/ PVOID WINAPI (*)(HANDLE, PVOID, SIZE_T, ULONG, ULONG, MEM_EXTENDED_PARAMETER*, ULONG);

VirtualAlloc2_func_t getVirtualAlloc2() {
  static auto funcVirtualAlloc2 = [] {
    LOG_USE_MODULE(memory);
    HMODULE h = GetModuleHandle("KernelBase");
    if (h != nullptr) {
      auto const addr = reinterpret_cast<VirtualAlloc2_func_t>(GetProcAddress(h, "VirtualAlloc2"));
      if (addr == nullptr) {
        LOG_CRIT(L"virtual_alloc2 == nullptr");
      }
      return addr;
    }
    LOG_CRIT(L"KernelBase not found");
    return (VirtualAlloc2_func_t) nullptr;
  }();

  return funcVirtualAlloc2;
}
} // namespace

namespace memory {
int getpagesize(void) {
  return util::getPageSize();
}

uint64_t getTotalSystemMemory() {
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);

  return status.ullTotalPhys;
}

uintptr_t reserve(uint64_t start, uint64_t size, uint64_t alignment, bool isGpu) {
  LOG_USE_MODULE(memory);

  MEM_ADDRESS_REQUIREMENTS requirements {
      .LowestStartingAddress = (PVOID)start,
      .HighestEndingAddress  = (PVOID)0,
      .Alignment             = alignment,
  };

  MEM_EXTENDED_PARAMETER param {
      .Type    = MemExtendedParameterAddressRequirements,
      .Pointer = &requirements,
  };

  auto ptr = (uintptr_t)getVirtualAlloc2()(GetCurrentProcess(), nullptr, size, isGpu ? MEM_RESERVE | MEM_WRITE_WATCH : MEM_RESERVE, PAGE_NOACCESS, &param, 1);
  if (ptr == 0) {
    auto err = static_cast<uint32_t>(GetLastError());
    if (err != ERROR_INVALID_PARAMETER) {
      LOG_ERR(L"reserve failed err:0x%08x| start:0x%08llx size:%llu alignment:%llu isGpu:%d", err, start, size, alignment, isGpu);
    } else {
      return reserve(start, size, alignment << 1, isGpu);
    }
  }

  return ptr;
}

uint64_t commit(uintptr_t baseAddr, uint64_t offset, uint64_t size, uint64_t alignment, int prot) {
  LOG_USE_MODULE(memory);

  auto ptr = (uintptr_t)VirtualAlloc((LPVOID)(baseAddr + offset), size, MEM_COMMIT, convProtection(prot));
  if (ptr == 0) {
    auto err = static_cast<uint32_t>(GetLastError());
    LOG_ERR(L"commit failed err:0x%0x| base:0x%08llx offset:0x%08llx size:%llu alignment:%llu prot:%d", err, baseAddr, offset, size, alignment, prot);
  }
  return ptr;
}

uint64_t allocGPUMemory(uintptr_t baseAddr, uint64_t offset, uint64_t size, uint64_t alignment) {
  LOG_USE_MODULE(memory);

  MEMORY_BASIC_INFORMATION im;
  VirtualQuery((LPVOID)baseAddr, &im, sizeof(im));

  VirtualFree(im.AllocationBase, 0, MEM_RELEASE);

  auto ptrBase = reserve((uint64_t)im.AllocationBase, im.RegionSize, alignment, true);
  if (ptrBase == 0) {
    auto err = static_cast<uint32_t>(GetLastError());
    LOG_ERR(L"allocGPUMemory failed err:0x%0x| addr:0x%08llx size:%llu", err, baseAddr, im.RegionSize);
    return 0;
  }

  auto ptr = (uintptr_t)VirtualAlloc((LPVOID)(baseAddr + offset), size, MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE);
  if (ptr == 0) {
    auto err = static_cast<uint32_t>(GetLastError());
    LOG_ERR(L"allocGPUMemory failed err:0x%0x| addr:0x%08llx size:%llu", err, baseAddr, size);
    return 0;
  }

  LOG_DEBUG(L"allocGPUMemory| base:0x%08llx(0x%08llx) addr:0x%08llx", ptrBase, baseAddr, ptr);
  return ptr;
}

int64_t queryAlloc(uintptr_t addr, uintptr_t* start, uintptr_t* end, int* prot) {
  MEMORY_BASIC_INFORMATION im;
  if (VirtualQuery((LPVOID)addr, &im, sizeof(im)) == 0) {
    return -1;
  }

  if (start != nullptr) *start = (uintptr_t)im.BaseAddress;
  if (end != nullptr) *end = *start + im.RegionSize;
  if (prot != nullptr) {
    *prot = convProtection(im.Protect);
  }
  return (int64_t)im.AllocationBase;
}

uint64_t allocAligned(uint64_t address, uint64_t size, int prot, uint64_t alignment) {
  LOG_USE_MODULE(memory);

  constexpr uint64_t USER_MIN = DIRECTMEM_START;
  constexpr uint64_t USER_MAX = 0xFBFFFFFFFFu;

  MEM_ADDRESS_REQUIREMENTS req2 {};
  MEM_EXTENDED_PARAMETER   param2 {};
  req2.LowestStartingAddress = (address == 0 ? reinterpret_cast<PVOID>(USER_MIN) : reinterpret_cast<PVOID>(address));
  req2.HighestEndingAddress  = reinterpret_cast<PVOID>(USER_MAX);
  req2.Alignment             = alignment;
  param2.Type                = MemExtendedParameterAddressRequirements;
  param2.Pointer             = &req2;

  static auto virtual_alloc2 = getVirtualAlloc2();

  if (virtual_alloc2 == nullptr) {
    LOG_CRIT(L"virtual_alloc2 == nullptr");
    return 0;
  }
  DWORD flags = static_cast<DWORD>(MEM_COMMIT) | static_cast<DWORD>(MEM_RESERVE);
  if (checkIsGPU(prot)) flags |= MEM_WRITE_WATCH;
  auto ptr = reinterpret_cast<uintptr_t>(virtual_alloc2(0, nullptr, size, flags, convProtection(prot), &param2, 1));

  if (ptr == 0) {
    auto err = static_cast<uint32_t>(GetLastError());
    if (err != ERROR_INVALID_PARAMETER) {
      LOG_ERR(L"VirtualAlloc2(alignment = 0x%08llx failed: 0x%04x", alignment, err);
    } else {
      return alloc(0, size, prot);
    }
  }
  return ptr;
}

uint64_t alloc(uint64_t address, uint64_t size, int prot) {
  LOG_USE_MODULE(memory);

  DWORD flags = static_cast<DWORD>(MEM_COMMIT) | static_cast<DWORD>(MEM_RESERVE);
  if (checkIsGPU(prot)) flags |= MEM_WRITE_WATCH;

  auto ptr = reinterpret_cast<uintptr_t>(VirtualAlloc(reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)), size, flags, convProtection(prot)));
  if (ptr == 0) {
    auto err = static_cast<uint32_t>(GetLastError());

    if (err != ERROR_INVALID_ADDRESS) {
      LOG_ERR(L"VirtualAlloc() failed @0x%08llx:0x%08llx protection:0x%x, err:0x%04x", address, size, prot, err);
    } else {
      return allocAligned(address, size, prot, 0);
    }
  }
  return ptr;
}

bool allocFixed(uint64_t address, uint64_t size, int protection) {
  LOG_USE_MODULE(memory);
  DWORD flags = static_cast<DWORD>(MEM_COMMIT) | static_cast<DWORD>(MEM_RESERVE);
  if (checkIsGPU(protection)) flags |= MEM_WRITE_WATCH;

  auto ptr = reinterpret_cast<uintptr_t>(VirtualAlloc(reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)), size, flags, convProtection(protection)));
  if (ptr == 0) {
    auto err = static_cast<uint32_t>(GetLastError());

    LOG_ERR(L"VirtualAlloc() failed: 0x%04x", err);

    return false;
  }

  if (ptr != address) {
    LOG_ERR(L"VirtualAlloc() failed: wrong address");
    VirtualFree(reinterpret_cast<LPVOID>(ptr), 0, MEM_RELEASE);
    return false;
  }

  return true;
}

bool free(uint64_t address) {
  LOG_USE_MODULE(memory);
  if (VirtualFree(reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)), 0, MEM_RELEASE) == 0) {
    LOG_ERR(L"VirtualFree() failed: 0x%04x", static_cast<uint32_t>(GetLastError()));
    return false;
  }
  return true;
}

bool protect(uint64_t address, uint64_t size, int protection, int* oldProt) {
  LOG_USE_MODULE(memory);

  DWORD oldProtection;
  if (VirtualProtect(reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)), size, convProtection(protection), &oldProtection) == 0) {
    LOG_ERR(L"VirtualProtect() failed addr:0x%08llx size:0x%08llx prot:%d err:0x%04x", address, size, protection, static_cast<uint32_t>(GetLastError()));
    return false;
  }

  if (oldProt != nullptr) *oldProt = (protection & 0xF0) | convProtection(oldProtection);
  return true;
}

int getProtection(uint64_t address) {
  LOG_USE_MODULE(memory);
  MEMORY_BASIC_INFORMATION mbi;
  if (!VirtualQuery((const void*)address, &mbi, sizeof(mbi))) {
    LOG_ERR(L"Failed to query memory");
    return 0;
  }
  return convProtection(mbi.Protect);
}

void installHook_long(uintptr_t dst, uintptr_t src, _t_hook& pGateway, size_t lenOpCodes) {
  std::array<uint8_t, 14> codeGateway = {
      0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,             // jmp qword
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // addr
  };

  // setup jmp back code
  *(uint64_t*)&codeGateway[6] = (uint64_t)(src + lenOpCodes);

  LOG_USE_MODULE(memory);
  {
    // Interceptor, jmp to dst
    std::array code      = codeGateway;
    *(uint64_t*)&code[6] = (uint64_t)(dst);

    int oldMode;
    protect(src, code.size(), SceProtExecute | SceProtRead | SceProtWrite, &oldMode);

    memcpy(pGateway.data.data(), (uint8_t*)src, lenOpCodes); // save code -> add to gateway
    memcpy((uint8_t*)src, code.data(), code.size());

    if (::FlushInstructionCache(GetCurrentProcess(), (void*)src, code.size()) == 0) {
      LOG_ERR(L"FlushInstructionCache() failed: 0x%04x", static_cast<uint32_t>(GetLastError()));
      return;
    }
    protect(src, code.size(), oldMode);
    // - interceptor
  }

  memcpy(pGateway.data.data() + lenOpCodes, (uint8_t*)codeGateway.data(), codeGateway.size()); // cpy to gateway

  protect((uintptr_t)pGateway.data.data(), pGateway.data.size(), SceProtExecute | SceProtRead, nullptr);
}
} // namespace memory