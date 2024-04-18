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

EXPORT SYSV_ABI int32_t sceVoiceCreatePort(uint32_t* portId, const SceVoicePortParam* pArg) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceDeletePort(uint32_t portId) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceConnectIPortToOPort(uint32_t ips, uint32_t ops) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceDisconnectIPortFromOPort(uint32_t ips, uint32_t ops) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceGetPortInfo(uint32_t portId, SceVoiceBasePortInfo* pInfo) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceGetResourceInfo(SceVoiceResourceInfo* pInfo) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceReadFromOPort(uint32_t ops, void* data, uint32_t* size) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceGetVolume(uint32_t portId, float* volume) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceGetMuteFlag(uint32_t portId, bool* bMuted) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceSetVolume(uint32_t portId, float volume) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceSetMuteFlag(uint32_t portId, bool bMuted) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceResumePort(uint32_t portId) {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceResumePortAll() {
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

EXPORT SYSV_ABI int32_t sceVoiceEnd() {
  LOG_USE_MODULE(libSceVoice);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
