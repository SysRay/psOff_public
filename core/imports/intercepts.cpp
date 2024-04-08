#define __APICALL_EXTERN
#include "imports_intercepts.h"
#undef __APICALL_EXTERN

#include "exports/runtimeExport.h"
#include "imports_runtime.h"

void core_initIntercepts() {
  /*Usage
  // functions has to be of SYSV_ABI!
  accessRuntimeExport()->interceptAdd((uintptr_t)int_Malloc, "Y7aJ1uydPMo", "libc", "libc")

  // Calling original
  auto const origFunction = accessRuntimeExport()->interceptGetAddr((uintptr_t)int_Malloc);
  typedef SYSV_ABI void* (*fcnPtr)(void*, size_t);
  void* ret = ((fcnPtr)origFunction)(ptr, size);
  */
}