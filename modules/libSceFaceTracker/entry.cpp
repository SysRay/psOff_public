#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceFaceTracker);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceFaceTracker";

EXPORT SYSV_ABI int sceFaceTrackerInitialize(int width, int height, int rowstride, int maxTrackingNum,
	                                           uint32_t* threadFuncMutex, const SceFaceTrackerConfig* pConfig, void* work, int workSize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerFinalize(void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerUpdate(const unsigned char* imageData, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerSearchTarget(uint32_t* threadFuncMutex, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerGetResult(int trackId, SceFaceTrackerResult* result, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerStartTracking(int trackId, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerStopTracking(int trackId, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerReset(bool isClearRegisterUser, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerAddUserFeature(int userId, const void* featureData, int featureDataSize, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerRegisterUser(int userId, const void* featureData, int featureDataSize, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerUnregisterUser(int userId, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerGetUserFeature(int userId, const void** featureData, int* featureDataSize, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerGetUserIdByTrackId(int trackId, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerGetTrackIdByUserId(int trackId, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerRegisterStartTrackingCallback(SceFaceTrackerStartTrackingCallbackFunc func, void* callbackParam, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerRegisterFixUserIdCallback(SceFaceTrackerFixUserIdCallbackFunc func, void* callbackParam, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerRegisterLostTargetCallback(SceFaceTrackerLostTargetCallbackFunc func, void* callbackParam, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerRegisterRecoveryFromLostTargetCallback(SceFaceTrackerRecoveryFromLostTargetCallbackFunc func, void* callbackParam, void* work) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceTrackerRegisterStopTrackingCallback(SceFaceTrackerStopTrackingCallbackFunc func, void* callbackParam, void* work) {
  return Ok;
}
}