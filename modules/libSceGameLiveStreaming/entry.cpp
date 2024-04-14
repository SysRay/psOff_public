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
  LOG_ERR(L"todo %S", __FUNCTION__);
  glss->isOnAir = false;
  glss->userId  = 1;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceGameLiveStreamingGetCurrentStatus2(SceGameLiveStreamingStatus2* glss2) {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
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

EXPORT SYSV_ABI int32_t sceGameLiveStreamingTerminate() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
