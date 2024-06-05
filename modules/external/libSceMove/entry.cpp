#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceMove);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceMove";

EXPORT SYSV_ABI int sceMoveInit(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceMoveTerm(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceMoveOpen(SceUserServiceUserId userId, int32_t type, int32_t index) {
  LOG_USE_MODULE(libSceMove);
  LOG_DEBUG(L"open userId:%d type:%d index:%d", userId, type, index);
  return Ok;
}

EXPORT SYSV_ABI int sceMoveClose(int32_t handle) {
  return Ok;
}

EXPORT SYSV_ABI RetValues sceMoveReadStateRecent(int32_t handle, int64_t timestamp, SceMoveData* pData, int32_t* pNum) {
  *pData = SceMoveData();
  *pNum  = 0;
  return RetValues::NO_CONTROLLER_CONNECTED;
}

EXPORT SYSV_ABI RetValues sceMoveReadStateLatest(int32_t handle, SceMoveData* pData) {
  *pData = SceMoveData();
  return RetValues::NO_CONTROLLER_CONNECTED;
}

EXPORT SYSV_ABI int sceMoveSetLightSphere(int32_t handle, uint8_t r, uint8_t g, uint8_t b) {
  return Ok;
}

EXPORT SYSV_ABI RetValues sceMoveSetVibration(int32_t handle, uint8_t motor) {
  return RetValues::NO_CONTROLLER_CONNECTED;
}

EXPORT SYSV_ABI RetValues sceMoveGetDeviceInfo(int32_t handle, SceMoveDeviceInfo* pInfo) {
  *pInfo = SceMoveDeviceInfo();
  return RetValues::NO_CONTROLLER_CONNECTED;
}

EXPORT SYSV_ABI int sceMoveResetLightSphere(int32_t handle) {
  return Ok;
}
}