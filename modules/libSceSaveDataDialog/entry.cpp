#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSaveDataDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSaveDataDialog";

EXPORT SYSV_ABI int32_t sceSaveDataDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceSaveDataDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceSaveDataDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogGetResult(SceSaveDataDialogResult* result) {
  LOG_USE_MODULE(libSceSaveDataDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  result->result = Ok;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogOpen(const SceSaveDataDialogParam* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogClose(const SceSaveDataDialogCloseParam* closeParam) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogProgressBarInc(SceSaveDataDialogProgressBarTarget target, uint32_t delta) {
  LOG_USE_MODULE(libSceSaveDataDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogProgressBarSetValue(SceSaveDataDialogProgressBarTarget target, uint32_t rate) {
  LOG_USE_MODULE(libSceSaveDataDialog);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogIsReadyToDisplay() {
  return 1;
}
}