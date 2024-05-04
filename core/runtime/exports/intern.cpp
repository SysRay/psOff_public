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

void initIntercepts() {
  /*Usage
  // functions has to be of SYSV_ABI!
  accessRuntimeExport()->interceptAdd((uintptr_t)int_Malloc, "Y7aJ1uydPMo", "libc", "libc")

  // Calling original
  auto const origFunction = accessRuntimeExport()->interceptGetAddr((uintptr_t)int_Malloc);
  typedef SYSV_ABI void* (*fcnPtr)(void*, size_t);
  void* ret = ((fcnPtr)origFunction)(ptr, size);
  */
}
} // namespace intern