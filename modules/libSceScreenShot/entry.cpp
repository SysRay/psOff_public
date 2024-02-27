#include "common.h"
#include "types.h"

#include <logging.h>

LOG_DEFINE_MODULE(libSceScreenShot);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceScreenShot";

EXPORT SYSV_ABI int32_t sceScreenShotSetParam(const SceScreenShotParam* param) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceScreenShotSetOverlayImage(const char* filePath, int32_t offsetX, int32_t offsetY) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceScreenShotSetOverlayImageWithOrigin(const char* filePath, int32_t marginX, int32_t marginY, SceScreenShotOrigin origin) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceScreenShotDisable(void) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceScreenShotEnable(void) {
  return Ok;
}
}