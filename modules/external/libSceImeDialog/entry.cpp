#include "codes.h"
#include "common.h"
#include "internal/oscctl/oscctl.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceImeDialog);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceImeDialog";

EXPORT SYSV_ABI int32_t sceImeDialogInit(const SceImeDialogParam* param, const SceImeParamExtended* ext) {
  LOG_USE_MODULE(libSceImeDialog);
  if (param == nullptr) return Err::ImeDialog::INVALID_ADDRESS;
  if (param->inputTextBuffer == nullptr) return Err::ImeDialog::INVALID_INPUT_TEXT_BUFFER;
  if (param->title == nullptr) return Err::ImeDialog::INVALID_TITLE;
  if (param->maxTextLength == 0) return Err::ImeDialog::INVALID_MAX_TEXT_LENGTH;
  auto& osc = accessOscCtl();

  if (param->type != SceImeType::DEFAULT) LOG_ERR(L"Handle ImeType");
  if (param->supportedLanguages) LOG_ERR(L"Handle supportedLanguages");
  if (param->inputMethod != SceImeInputMethod::DEFAULT) LOG_ERR(L"Handle inputMethod");
  if (param->filter) LOG_ERR(L"Handle TextFilter function");
  if (param->option) LOG_ERR(L"Handle ImeOption(s)");
  if (param->placeholder) LOG_ERR(L"Handle placeholder");

  if (!osc.run(param->title)) return Err::Ime::BUSY;

  auto setup = [&osc, param]() -> int32_t {
    if (!osc.setEnterLabel(param->enterLabel)) return Err::Ime::INVALID_ENTER_LABEL;
    if (!osc.setBuffer(param->inputTextBuffer, param->maxTextLength)) return Err::Ime::INVALID_INPUT_TEXT_BUFFER;
    if (!osc.show()) return Err::Ime::INTERNAL;
    return Ok;
  };

  int32_t ecode = setup();
  if (ecode != Ok) osc.destroy();
  return ecode;
}

EXPORT SYSV_ABI SceImeDialogStatus sceImeDialogGetStatus() {
  if (auto param = accessOscCtl().getParams()) {
    switch (param->status) {
      case IOscCtl::Status::HIDDEN:
      case IOscCtl::Status::SHOWN: return SceImeDialogStatus::RUNNING;

      case IOscCtl::Status::CANCELLED:
      case IOscCtl::Status::DONE: return SceImeDialogStatus::FINISHED;

      default: return SceImeDialogStatus::NONE;
    }
  }

  return SceImeDialogStatus::NONE;
}

EXPORT SYSV_ABI int32_t sceImeDialogTerm() {
  switch (sceImeDialogGetStatus()) {
    case SceImeDialogStatus::RUNNING: return Err::ImeDialog::NOT_FINISHED;
    case SceImeDialogStatus::NONE: return Err::ImeDialog::NOT_IN_USE;
    default: {
      accessOscCtl().destroy();
      return Ok;
    }
  }
}

EXPORT SYSV_ABI int32_t sceImeDialogAbort() {
  switch (sceImeDialogGetStatus()) {
    case SceImeDialogStatus::FINISHED: return Err::ImeDialog::NOT_RUNNING;
    case SceImeDialogStatus::NONE: return Err::ImeDialog::NOT_IN_USE;
    default: {
      accessOscCtl().abort();
      return Ok;
    }
  }
}

EXPORT SYSV_ABI int32_t sceImeDialogParamInit(SceImeDialogParam* param) {
  LOG_USE_MODULE(libSceImeDialog);
  LOG_ERR(L"todo %S", __FUNCTION__);
  ::memset(param, 0, sizeof(SceImeDialogParam));
  return Ok;
}

EXPORT SYSV_ABI int32_t sceImeDialogGetResult(SceImeDialogResult* res) {
  if (auto param = accessOscCtl().getParams()) {
    switch (param->status) {
      case IOscCtl::Status::DONE: {
        res->endstatus = SceImeDialogEndStatus::OK;
        return Ok;
      } break;
      case IOscCtl::Status::CANCELLED: {
        res->endstatus = param->aborted ? SceImeDialogEndStatus::ABORTED : SceImeDialogEndStatus::USER_CANCELED;
        return Ok;
      } break;

      default: return Err::ImeDialog::NOT_FINISHED;
    }
  }

  return Err::ImeDialog::NOT_IN_USE;
}

// EXPORT SYSV_ABI int32_t sceImeDialogGetPanelSizeExtended(const SceImeDialogParam* param, const SceImeParamExtended* ext, uint32_t* w, uint32_t* h) {
//   return Ok;
// }

// EXPORT SYSV_ABI int32_t sceImeDialogGetPanelPositionAndForm(SceImePositionAndForm* pf) {
//   return Ok;
// }
}
