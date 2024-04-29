#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpParty);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpParty";

EXPORT SYSV_ABI int32_t sceNpPartyInitialize() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyCheckCallback() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyCreate() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyGetId() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyGetMemberInfo() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyGetMemberVoiceInfo() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyGetMembers() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyGetState() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyGetStateAsUser() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyRegisterHandler() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartySendBinaryMessage() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyShowInvitationList() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyTerminate() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
