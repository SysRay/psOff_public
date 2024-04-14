#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpTus);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpTus";

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusCreateNpTitleCtx() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusCreateRequest() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteNpTitleCtx() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteRequest() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusPollAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableAsync() {
  return Ok;
}
}
