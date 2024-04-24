#include "codes.h"
#include "common.h"
#include "logging.h"

LOG_DEFINE_MODULE(libSceNpGameIntent);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpGameIntent";

EXPORT SYSV_ABI int sceNpGameIntentInitialize() {
  LOG_USE_MODULE(libSceNpGameIntent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
