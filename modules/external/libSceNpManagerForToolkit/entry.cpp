#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpManagerForToolkit);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpManager";

EXPORT SYSV_ABI int sceNpCheckCallbackForLib() {
  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterStateCallbackForToolkit(void* /*callback*/, void* /*userdata*/) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterStateCallbackForToolkit(void* /*callback*/, void* /*userdata*/) {
  return Ok;
}
}