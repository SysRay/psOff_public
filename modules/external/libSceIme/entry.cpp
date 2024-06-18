#include "common.h"
#include "logging.h"
#include "types.h"

#include <SDL2/SDL.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>

LOG_DEFINE_MODULE(libSceIme);

namespace {
struct ImeHandler {
  uint32_t             state;
  int32_t              userId;
  SceImeEventHandler   func;
  void*                arg;
  std::vector<uint8_t> prevstate;
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
      SceImeEvent ev = {};

      switch (handler.state) {
        case 0: { // Not initialized
          ev.id                    = Ime::Event::KEYBOARD_EVENT_OPEN;
          ev.param.resourceIdArray = {
              .userId     = handler.userId,
              .resourceId = {1, 1, 1, 1, 1},
          };
          func(handler.arg, &ev);
          handler.state = 1; // Switch to running state
          continue;
        } break;

        case 1: { // Running
          int32_t length;

          const uint8_t* currstate = SDL_GetKeyboardState(&length);
          if (handler.prevstate.size() < length) handler.prevstate.resize(length);

          auto ts = (uint64_t)boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds();
          for (uint16_t c = 0; c < length; ++c) {
            if (currstate[c] != handler.prevstate[c]) {
              ev.id    = currstate[c] < 1 ? Ime::Event::KEYBOARD_KEYCODE_UP : Ime::Event::KEYBOARD_KEYCODE_DOWN;
              ev.param = {
                  .keycode =
                      {
                          .keycode    = c, // All the SCE keycodes and SDL scancodes the same
                          .character  = L'\000',
                          .status     = 1, // Valid keycode is stored
                          .type       = SceImeKeyboardType::ENGLISH_US,
                          .userId     = handler.userId,
                          .resourceId = 1,
                          .timestamp  = {.tick = ts},
                      },
              };
              func(handler.arg, &ev);
            }
          }

          ::memcpy(handler.prevstate.data(), currstate, length);
          continue;
        } break;
      }

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
  g_handlers.push_back({0, userId, param->handler, param->arg});
  return Ok;
}

EXPORT SYSV_ABI int sceImeKeyboardClose(SceUserServiceUserId userId) {
  for (auto it = g_handlers.begin(); it != g_handlers.end(); ++it) {
    if (it->userId == userId) {
      g_handlers.erase(it);
      return Ok;
    }
  }

  return Err::Ime::NOT_OPENED;
}

EXPORT SYSV_ABI int sceImeKeyboardGetResourceId(SceUserServiceUserId userId, SceImeKeyboardResourceIdArray* resourceIdArray) {
  return Ok;
}

EXPORT SYSV_ABI int sceImeKeyboardGetInfo(uint32_t resourceId, SceImeKeyboardInfo* info) {
  info->userId      = -1;
  info->device      = SceImeKeyboardDeviceType::USB_KEYBOARD;
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
