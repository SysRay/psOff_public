#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceInvitationDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceInvitationDialog";

EXPORT SYSV_ABI int32_t sceInvitationDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceInvitationDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceInvitationDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceInvitationDialogOpen(const void* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}
}
