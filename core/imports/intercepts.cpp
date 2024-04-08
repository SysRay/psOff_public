#define __APICALL_EXTERN
#include "imports_intercepts.h"
#undef __APICALL_EXTERN

#include "exports/runtimeExport.h"
#include "imports_runtime.h"

void core_initIntercepts() {
  /*Usage
  accessRuntimeExport()->interceptAdd((uintptr_t)int_Malloc, "Y7aJ1uydPMo", "libc", "libc")

  // Calling original
  auto const origFunction = accessRuntimeExport()->interceptGetAddr((uintptr_t)int_Malloc);
  typedef void* (*fcnPtr)(void*, size_t);
  void* ret = ((fcnPtr)origFunction)(ptr, size);
  */
}