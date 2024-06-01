#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceGameLiveStreaming);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceGameLiveStreaming";

EXPORT SYSV_ABI int32_t sceGameLiveStreamingInitialize() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingPermitServerSideRecording() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingPermitLiveStreaming() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingGetCurrentStatus(SceGameLiveStreamingStatus* glss) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  glss->isOnAir = false;
  glss->userId  = 1;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingGetCurrentStatus2(SceGameLiveStreamingStatus2* glss2) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  glss2->isOnAir = false;
  glss2->userId  = 1;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingEnableLiveStreaming(bool isStreamActive) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingSetGuardAreas(size_t areaCount, const SceGameLiveStreamingGuardArea* areas) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingSetMaxBitrate(const unsigned int maxBitrate) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingSetCameraFrameSetting(const SceGameLiveStreamingCameraFrameSetting* set) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingGetCurrentBroadcastScreenLayout(SceGameLiveStreamingBroadcastScreenLayout* layout) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingApplySocialFeedbackMessageFilter(const size_t maxCount, SceGameLiveStreamingSocialFeedbackMessageFilter filter) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingClearPresetSocialFeedbackCommands() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingClearSpoilerTag() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingClearSocialFeedbackMessages(const SceGameLiveStreamingSocialFeedbackMessageType type) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingSetStandbyScreenResource(const char* resource) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingTerminate() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
