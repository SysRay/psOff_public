#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceErrorDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceErrorDialog";

EXPORT SYSV_ABI int32_t sceErrorDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceErrorDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceErrorDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceErrorDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceSaveDataDialogOpen(const void* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}
}
