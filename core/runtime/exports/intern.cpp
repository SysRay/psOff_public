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

void init() {
  LIB_DEFINE_START("intern", 0, "intern", 0, 0)
  LIB_FUNC("module_start", module_start), LIB_DEFINE_END();
}
} // namespace intern