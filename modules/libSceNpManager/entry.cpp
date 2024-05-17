#include "common.h"
#include "logging.h"
#include "types.h"

#include <utility/utility.h>

LOG_DEFINE_MODULE(libSceNpManager);

namespace {
using SceNpStateCallbackA            = SYSV_ABI void (*)(int32_t userId, SceNpState state, void* userdata);
using SceNpReachabilityStateCallback = SYSV_ABI void (*)(int32_t userId, SceNpReachabilityState state, void* userdata);
using SceNpGamePresenceCallbackA     = SYSV_ABI void (*)(int32_t userId, SceNpGamePresenceStatus status, void* userdata);
using SceNpPlusEventCallback         = SYSV_ABI void (*)(int32_t userId, SceNpPlusEventType event, void* userdata);
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpManager";

EXPORT SYSV_ABI int sceNpSetNpTitleId(const SceNpTitleId* titleId, const SceNpTitleSecret* titleSecret) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpSetContentRestriction(const SceNpContentRestriction* pRestriction) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpCheckCallback(void) {
  // todo iterate callbacks
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetState(int32_t userId, SceNpState* state) {
  *state = SceNpState::SIGNED_OUT;
  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterStateCallback(SceNpStateCallbackA callback, void* userdata) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterStateCallback(int callbackId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterStateCallbackA(SceNpStateCallbackA callback, void* userdata) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterStateCallbackA(int callbackId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetNpReachabilityState(int32_t userId, SceNpReachabilityState* state) {
  *state = SceNpReachabilityState::UNAVAILABLE;
  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterNpReachabilityStateCallback(SceNpReachabilityStateCallback callback, void* userdata) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterNpReachabilityStateCallback(void) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpHasSignedUp(int32_t userId, bool* hasSignedUp) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *hasSignedUp = true;
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountId(int32_t userId, SceNpAccountId* accountId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *accountId = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountIdA(int32_t userId, SceNpAccountId* accountId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *accountId = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetUserIdByAccountId(SceNpAccountId accountId, int32_t* userId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *userId = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetOnlineId(int32_t userId, SceNpOnlineId* onlineId) {
  auto const count      = std::string("onlineId").copy(onlineId->data, 15);
  onlineId->data[count] = '\0';
  onlineId->term        = 0;

  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S, userId:%d", __FUNCTION__, userId);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetNpId(int32_t userId, SceNpId* npId) {
  auto const count         = std::string("NpIdName").copy(npId->handle.data, 15);
  npId->handle.data[count] = '\0';
  npId->handle.term        = 0;
  LOG_USE_MODULE(libSceNpManager);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetPlatformType(const SceNpId* npId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return (int)SceNpPlatformType::TYPE_PS4;
}

EXPORT SYSV_ABI int sceNpSetPlatformType(SceNpId* npId, SceNpPlatformType platformType) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountCountry(int32_t userId, SceNpCountryCode* countryCode) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountDateOfBirth(int32_t userId, SceNpDate* dateOfBirth) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);

  dateOfBirth->year  = 1990;
  dateOfBirth->month = 1;
  dateOfBirth->day   = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterGamePresenceCallback(SceNpGamePresenceCallbackA callback, void* userdata) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterGamePresenceCallback(int callbackId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetGamePresenceStatus(int32_t userId, SceNpGamePresenceStatus* status) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *status = SceNpGamePresenceStatus::OFFLINE;
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountCountryA(int32_t userId, SceNpCountryCode* countryCode) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);

  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountDateOfBirthA(int32_t userId, SceNpDate* dateOfBirth) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterGamePresenceCallbackA(SceNpGamePresenceCallbackA callback, void* userdata) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterGamePresenceCallbackA(int callbackId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetGamePresenceStatusA(int32_t userId, SceNpGamePresenceStatus* status) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *status = SceNpGamePresenceStatus::OFFLINE;
  return Ok;
}

EXPORT SYSV_ABI int sceNpCreateRequest(void) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  static int32_t id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceNpCreateAsyncRequest(const SceNpCreateAsyncRequestParameter* param) {

  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  static int32_t id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceNpDeleteRequest(int reqId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpAbortRequest(int reqId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpSetTimeout(int reqId, int32_t resolveRetry, uint32_t resolveTimeout, uint32_t connTimeout, uint32_t sendTimeout,
                                    uint32_t recvTimeout) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpWaitAsync(int reqId, int* result) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpPollAsync(int reqId, int* result) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *result = 0;
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountLanguage(int reqId, int32_t userId, SceNpLanguageCode* langCode) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetParentalControlInfo(int reqId, int32_t userId, int8_t* age, SceNpParentalControlInfo* info) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpCheckNpAvailability(int reqId, int32_t userId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpCheckNpReachability(int reqId, int32_t userId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpSetGamePresenceOnline(int reqId, int32_t userId, uint32_t timeout, void* pReserved) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetAccountLanguageA(int reqId, int32_t userId, SceNpLanguageCode* langCode) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpGetParentalControlInfoA(int reqId, int32_t userId, int8_t* age, SceNpParentalControlInfo* info) {
  info->chatRestriction    = false;
  info->contentRestriction = false;
  info->ugcRestriction     = false;
  *age                     = 18;
  return Ok;
}

EXPORT SYSV_ABI int sceNpCheckNpAvailabilityA(int reqId, int32_t userId) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpSetGamePresenceOnlineA(int reqId, int32_t userId, uint32_t timeout, void* pReserved) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpCheckPlus(int reqId, const SceNpCheckPlusParameter* param, SceNpCheckPlusResult* pResult) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  pResult->authorized = false;

  return Ok;
}

EXPORT SYSV_ABI int sceNpRegisterPlusEventCallback(SceNpPlusEventCallback callback, void* userdata) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpUnregisterPlusEventCallback(void) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpNotifyPlusFeature(const SceNpNotifyPlusFeatureParameter* param) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpInGameMessageInitialize(size_t poolSize, void* option) {
  LOG_USE_MODULE(libSceNpManager);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
