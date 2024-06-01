#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceZlib);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceZlib";

EXPORT SYSV_ABI int32_t sceZlibInitialize(const void* buffer, size_t length) {
  LOG_USE_MODULE(libSceZlib);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}