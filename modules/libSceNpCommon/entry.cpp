#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpCommon);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpCommon";

EXPORT SYSV_ABI void __DUMMY(FUNCTION_1)() {}

EXPORT SYSV_ABI int32_t sceNpCmpNpId() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMutexInit() {
  return Ok;
}
}
