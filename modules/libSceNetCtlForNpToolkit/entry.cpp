#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNetCtlForNpToolkit);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNetCtl";

EXPORT SYSV_ABI int sceNetCtlRegisterCallbackForNpToolkit() {
  return Ok;
}
}
