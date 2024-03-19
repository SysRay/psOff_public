#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpAuth);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpAuth";

EXPORT SYSV_ABI int32_t sceNpAuthAbortRequest() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthCreateAsyncRequest() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthCreateRequest() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthDeleteRequest() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthGetAuthorizationCode() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthGetAuthorizationCodeA() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthPollAsync() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthSetTimeout() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthWaitAsync() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpAuthGetIdTokenA() {
  LOG_USE_MODULE(libSceNpAuth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
