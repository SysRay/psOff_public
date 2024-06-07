#include "codes.h"
#include "common.h"
#include "internal/oscctl/oscctl.h"
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

  auto& osc = accessOscCtl();

  if (!osc.run()) return Err::Ime::BUSY;

  auto setup = [&osc, param]() -> int32_t {
    if (!osc.setEnterLabel(param->enterLabel)) return Err::Ime::INVALID_ENTER_LABEL;
    if (!osc.setBuffer(param->inputTextBuffer, param->maxTextLength))
      if (!osc.show()) return Err::Ime::INTERNAL;
    return Ok;
  };

  int32_t ecode = setup();
  if (ecode != Ok) osc.destroy();
  return ecode;
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
  if (auto param = accessOscCtl().getParams()) {
    switch (param->status) {
      case IOscCtl::Status::HIDDEN:
      case IOscCtl::Status::SHOWN: return SceImeDialogStatus::RUNNING;

      case IOscCtl::Status::CLOSED: return SceImeDialogStatus::FINISHED;

      default: return SceImeDialogStatus::NONE;
    }
  }

  return SceImeDialogStatus::NONE;
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
