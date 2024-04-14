#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceUlt);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceUlt";

EXPORT SYSV_ABI int32_t sceUltInitialize() {
  return Ok;
}
}