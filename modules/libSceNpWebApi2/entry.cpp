#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpWebApi2);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpWebApi2";

EXPORT SYSV_ABI int32_t sceNpWebApi2Initialize() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApi2CreateRequest() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApi2AddHttpRequestHeader() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApi2SendRequest() {
  return Ok;
}
}
