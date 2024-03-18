#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceMsgDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceMsgDialog";

EXPORT SYSV_ABI int32_t sceMsgDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMsgDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceMsgDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceMsgDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceMsgDialogGetResult(SceMsgDialogResult* result) {
  LOG_USE_MODULE(libSceMsgDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  result->result = Ok;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMsgDialogOpen(const SceMsgDialogParam* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMsgDialogClose() {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMsgDialogProgressBarInc(uint32_t target, uint32_t delta) {
  LOG_USE_MODULE(libSceMsgDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMsgDialogProgressBarSetValue(uint32_t target, uint32_t rate) {
  LOG_USE_MODULE(libSceMsgDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMsgDialogProgressBarSetMsg() {
  LOG_USE_MODULE(libSceMsgDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}
}
