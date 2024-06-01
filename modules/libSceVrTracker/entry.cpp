#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceVrTracker);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceVrTracker";

EXPORT SYSV_ABI int32_t sceVrTrackerInit(const SceVrTrackerInitParam* param) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVrTrackerQueryMemory(const SceVrTrackerQueryMemoryParam* param, SceVrTrackerQueryMemoryResult* result) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVrTrackerRegisterDevice(SceVrTrackerDeviceType deviceType, int32_t handle) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVrTrackerUnregisterDevice(int32_t handle) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVrTrackerSetDurationUntilStatusNotTracking(SceVrTrackerDeviceType deviceType, uint32_t durationCameraFrames) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVrTrackerGpuSubmit(const SceVrTrackerGpuSubmitParam* param) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVrTrackerTerm(void) {
  LOG_USE_MODULE(libSceVrTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
