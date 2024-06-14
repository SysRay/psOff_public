#define __APICALL_EXTERN
#include "intern.h"
#undef __APICALL_EXTERN

#include "../runtimeLinker.h"
#include "macro.h"
#include "utility/utility.h"

namespace intern {

SYSV_ABI void module_start(size_t argc, const void* argp, int* pRes) {
  // todo ?
}

void initIntercepts();

void init() {
  LIB_DEFINE_START("intern", 0, "intern", 0, 0)
  LIB_FUNC("module_start", module_start), LIB_DEFINE_END();

  initIntercepts();
}

/*
void SYSV_ABI logfunc(const char* str, void* args) {
  char sanitize[1024] = "gamelogger| ";
  for (int i = 12; i < 1024; ++i) {
    if (*str >= ' ' && *str <= '~')
      sanitize[i] = *str++;
    else {
      sanitize[i]     = '\n';
      sanitize[i + 1] = '\0';
      break;
    }
  }
  vprintf(sanitize, (va_list)&args);
}
*/

void post() {
  /*
    // NieR: Automata v01.06 internal game logger interception example:
    auto mp = accessRuntimeLinker().accessMainProg();
    accessRuntimeLinker().interceptInternal(mp, 0xbe09e0, (uint64_t)&logfunc);
  */
}

void initIntercepts() {
  /*Usage
  // functions has to be of SYSV_ABI!
  accessRuntimeLinker().interceptAdd((uintptr_t)int_Malloc, "Y7aJ1uydPMo", "libc", "libc")

  // Calling original
  auto const origFunction = accessRuntimeLinker().interceptGetAddr((uintptr_t)int_Malloc);
  typedef SYSV_ABI void* (*fcnPtr)(void*, size_t);
  void* ret = ((fcnPtr)origFunction)(ptr, size);
  */
}
} // namespace intern
