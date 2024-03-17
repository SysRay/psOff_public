#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceGameLiveStreaming);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceGameLiveStreaming";

EXPORT SYSV_ABI int32_t __NID(sceGameLiveStreamingInitialize)() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID(sceGameLiveStreamingPermitServerSideRecording)() {
  LOG_USE_MODULE(libSceGameLiveStreaming);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
