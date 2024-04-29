#include "virtualmemory.h"

#include "core/memory/memory.h"
#include "logging.h"

#include <windows.h>

LOG_DEFINE_MODULE(VIRTUALMEMORY);

namespace {} // namespace

bool flushInstructionCache(uint64_t address, uint64_t size) {
  LOG_USE_MODULE(VIRTUALMEMORY);
  if (::FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPVOID>(static_cast<uintptr_t>(address)), size) == 0) {
    LOG_ERR(L"FlushInstructionCache() failed: 0x%04x", static_cast<uint32_t>(GetLastError()));
    return false;
  }
  return true;
}

bool patchReplace(uint64_t vaddr, uint64_t const value) {
  int oldMode;
  memory::protect(vaddr, 8, SceProtRead | SceProtWrite, &oldMode);

  auto* ptr = reinterpret_cast<uint64_t*>(vaddr);

  bool ret = (*ptr != value);

  *ptr = value;

  memory::protect(vaddr, 8, oldMode);

  if (memory::isExecute(oldMode)) {
    flushInstructionCache(vaddr, 8);
  }

  return ret;
}