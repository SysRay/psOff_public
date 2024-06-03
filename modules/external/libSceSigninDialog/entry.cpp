#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSigninDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSigninDialog";

EXPORT SYSV_ABI int32_t sceSigninDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSigninDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceSigninDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceSigninDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceSigninDialogGetResult(void) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSigninDialogOpen(const void* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSigninDialogClose(void) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}
}
