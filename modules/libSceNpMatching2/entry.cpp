#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpMatching2);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpMatching2";

EXPORT SYSV_ABI int32_t sceNpMatching2Initialize() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2RegisterContextCallback() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2RegisterLobbyEventCallback() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2RegisterRoomEventCallback() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2RegisterRoomMessageCallback() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2RegisterSignalingCallback() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2ContextStart() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2CreateContext() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2CreateContextA() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2SetDefaultRequestOptParam() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpMatching2Terminate() {
  LOG_USE_MODULE(libSceNpMatching2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
