#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceMouse);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceMouse";

EXPORT SYSV_ABI int sceMouseInit(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceMouseOpen(SceUserServiceUserId userId, int32_t type, int32_t index, const SceMouseOpenParam* pParam) {
  return Ok;
}

EXPORT SYSV_ABI int sceMouseClose(int32_t handle) {
  return Ok;
}

EXPORT SYSV_ABI int sceMouseRead(int32_t handle, SceMouseData* pData, int32_t num) {
  pData[0] = SceMouseData();
  return 1;
}
}