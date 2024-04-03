#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceCamera);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceCamera";

EXPORT SYSV_ABI int32_t sceCameraOpen() {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

}
