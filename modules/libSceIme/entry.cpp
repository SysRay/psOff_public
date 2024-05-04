#include "common.h"
#include "logging.h"
#include "types.h"

#include <SDL2/SDL.h>
#include <vector>

LOG_DEFINE_MODULE(libSceIme);

namespace {
struct ImeHandler {
  int32_t            userId;
  SceImeEventHandler func;
  char               kbdstate[256];
};

std::vector<ImeHandler> g_handlers = {};
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceIme";

EXPORT SYSV_ABI int sceImeOpen(const SceImeParam* param, const SceImeParamExtended* extParam) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeUpdate(SceImeEventHandler func) {
  for (auto& handler: g_handlers) {
    if (handler.func == func) {
      int32_t length;
      auto    currstate = SDL_GetKeyboardState(&length);
      return Ok;
    }
  }

  return Err::Ime::NOT_OPENED;
}

EXPORT SYSV_ABI int sceImeSetText(const wchar_t* text, uint32_t length) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeSetCaret(const SceImeCaret* caret) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeSetTextGeometry(SceImeTextAreaMode mode, const SceImeTextGeometry* geometry) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeGetPanelSize(const SceImeParam* param, uint32_t* width, uint32_t* height) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeClose(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeGetPanelPositionAndForm(SceImePositionAndForm* posForm) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeSetCandidateIndex(int32_t index) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeConfirmCandidate(int32_t index) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeDisableController(void) {
  return Ok;
}

EXPORT SYSV_ABI void sceImeParamInit(SceImeParam* param) {}

EXPORT SYSV_ABI int sceImeKeyboardOpen(SceUserServiceUserId userId, const SceImeKeyboardParam* param) {
  if (param == nullptr) return Err::Ime::INVALID_ARG;
  if (param->handler == nullptr) return Err::Ime::INVALID_HANDLER;
  for (auto& handler: g_handlers) {
    if (handler.userId == userId) return Err::Ime::BUSY;
  }
  g_handlers.push_back({userId, param->handler});
  return Ok;
}

EXPORT SYSV_ABI int sceImeKeyboardClose(SceUserServiceUserId userId) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeKeyboardGetResourceId(SceUserServiceUserId userId, SceImeKeyboardResourceIdArray* resourceIdArray) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeKeyboardGetInfo(uint32_t resourceId, SceImeKeyboardInfo* info) {
  info->userId      = -1;
  info->device      = SceImeKeyboardDeviceType::KEYBOARD;
  info->type        = SceImeKeyboardType::ENGLISH_US;
  info->repeatDelay = 1;
  info->repeatRate  = 1;
  info->status      = SceImeKeyboardStatus::DISCONNECTED;

  return Ok;
}

EXPORT SYSV_ABI int sceImeKeyboardSetMode(SceUserServiceUserId userId, uint32_t mode) {
  return Ok;
}
}
