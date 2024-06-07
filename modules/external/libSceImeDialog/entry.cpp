#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceImeDialog);

namespace {
static SceImeDialogStatus g_curStatus = SceImeDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceImeDialog";

EXPORT SYSV_ABI int32_t sceImeDialogInit(const SceImeDialogParam* param, const SceImeParamExtended* ext) {
  g_curStatus = SceImeDialogStatus::RUNNING;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogTerm() {
  g_curStatus = SceImeDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogAbort() {
  g_curStatus = SceImeDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogParamInit(SceImeDialogParam* param) {
  LOG_USE_MODULE(libSceImeDialog);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI SceImeDialogStatus sceImeDialogGetStatus() {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceImeDialogGetPanelSizeExtended(const SceImeDialogParam* param, const SceImeParamExtended* ext, uint32_t* w, uint32_t* h) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogGetResult(SceImeDialogResult* res) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogGetPanelPositionAndForm(SceImePositionAndForm* pf) {
  return Ok;
}
}
