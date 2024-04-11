#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceVoice);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceVoice";

EXPORT SYSV_ABI int32_t sceVoiceInit() {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceSetThreadsParams() {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceStart() {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
