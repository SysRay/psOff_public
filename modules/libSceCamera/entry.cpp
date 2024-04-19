#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceCamera);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceCamera";

EXPORT SYSV_ABI int32_t sceCameraOpen(SceUserServiceUserId userId, int32_t type, int32_t index, SceCameraOpenParam* param) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraIsAttached(int32_t index) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 0;
}

EXPORT SYSV_ABI int32_t sceCameraGetConfig(int32_t handle, SceCameraConfig* pConfig) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraSetConfig(int32_t handle, SceCameraConfig* pConfig) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraSetVideoSync(int32_t handle, SceCameraVideoSyncParameter* pVideoSync) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraStart(int32_t handle, SceCameraStartParameter* pParam) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraGetFrameData(int32_t handle, SceCameraFrameData* pFrameData) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraSetAttribute(int32_t handle, SceCameraAttribute* pAttribute) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraStop(int32_t handle) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCameraClose(int32_t handle) {
  LOG_USE_MODULE(libSceCamera);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
