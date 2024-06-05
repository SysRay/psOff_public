#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpSnsFacebookDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpSnsFacebookDialog";

EXPORT SYSV_ABI int32_t sceNpSnsFacebookDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSnsFacebookDialogTerminate() {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSnsFacebookDialogOpen() {
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceNpSnsFacebookDialogGetStatus() {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceNpSnsFacebookDialogUpdateStatus() {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceNpSnsFacebookDialogGetResult() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSnsFacebookDialogClose() {
  return Ok;
}
}
