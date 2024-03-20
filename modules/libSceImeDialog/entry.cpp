#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceImeDialog);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceImeDialog";

EXPORT SYSV_ABI int32_t sceImeDialogInit() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogTerm(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceImeDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceImeDialogGetResult() {
  return Ok;
}
}
