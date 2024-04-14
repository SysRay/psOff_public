#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpCommon);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpCommon";

EXPORT SYSV_ABI int32_t sceNpMutexInit() {
  return Ok;
}
}