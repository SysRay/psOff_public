#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSocialScreen);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSocialScreen";

EXPORT SYSV_ABI int32_t sceSocialScreenInitialize() {
  LOG_USE_MODULE(libSceSocialScreen);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSocialScreenTerminate() {
  LOG_USE_MODULE(libSceSocialScreen);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}