#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceHttp2);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceHttp2";

EXPORT SYSV_ABI int32_t sceHttp2Init() {
  return Ok;
}
}
