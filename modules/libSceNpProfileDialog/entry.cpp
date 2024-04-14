#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpProfileDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpProfileDialog";

EXPORT SYSV_ABI int32_t sceNpProfileDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpProfileDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceNpProfileDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceNpProfileDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceNpProfileDialogOpen(const void* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpProfileDialogOpenA(const void* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpProfileDialogClose() {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}
}
