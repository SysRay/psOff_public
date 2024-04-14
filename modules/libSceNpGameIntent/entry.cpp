#include "common.h"
#include "errorcodes.h"
#include "logging.h"

LOG_DEFINE_MODULE(libSceNpGameIntent);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpGameIntent";

EXPORT SYSV_ABI int __NID_HEX(9BCEC11F1B7F1FAD)() {
  LOG_USE_MODULE(libSceNpGameIntent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
