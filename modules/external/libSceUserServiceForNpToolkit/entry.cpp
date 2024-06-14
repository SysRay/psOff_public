#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceUserServiceForNpToolkit);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceUserService";

EXPORT SYSV_ABI int sceUserServiceRegisterEventCallback() {
  return Ok;
}
}
