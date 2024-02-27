#include "common.h"
#include "errorcodes.h"

#include <logging.h>

LOG_DEFINE_MODULE(libSceCommonDialog);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceCommonDialog";

EXPORT SYSV_ABI int32_t sceCommonDialogInitialize() {
  LOG_USE_MODULE(libSceCommonDialog);
  LOG_TRACE(L"%S", __FUNCTION__);

  return Ok;
}

EXPORT SYSV_ABI bool sceCommonDialogIsUsed() {
  LOG_USE_MODULE(libSceCommonDialog);
  LOG_TRACE(L"%S", __FUNCTION__);

  return false;
}
}