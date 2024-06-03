#pragma once

#include "utility/utility.h"

#include <SDL2/SDL.h>
#include <functional>

enum class HkCommand {
  UNKNOWN = 0,

  // System commands
  GAMEREPORT_SEND_REPORT,
  OVERLAY_MENU,

  // Gamepad emulation
  CONTROLLER_L3,
  CONTROLLER_R3,
  CONTROLLER_OPTIONS,
  CONTROLLER_DPAD_UP,
  CONTROLLER_DPAD_RIGHT,
  CONTROLLER_DPAD_DOWN,
  CONTROLLER_DPAD_LEFT,
  CONTROLLER_L2,
  CONTROLLER_R2,
  CONTROLLER_L1,
  CONTROLLER_R1,
  CONTROLLER_TRIANGLE,
  CONTROLLER_CIRCLE,
  CONTROLLER_CROSS,
  CONTROLLER_SQUARE,
  CONTROLLER_TOUCH_PAD,
  CONTROLLER_LX_UP,
  CONTROLLER_LX_DOWN,
  CONTROLLER_LY_UP,
  CONTROLLER_LY_DOWN,
  CONTROLLER_RX_UP,
  CONTROLLER_RX_DOWN,
  CONTROLLER_RY_UP,
  CONTROLLER_RY_DOWN,

  /**
   * Warning! If this value ever exceeds 32 you should increase
   * the size of `m_state` variable in `class Hotkeys` aswell!
   */
  COMMANDS_MAX,
};

typedef std::function<void(HkCommand)> HkFunc;

class IHotkeys {
  CLASS_NO_COPY(IHotkeys);
  CLASS_NO_MOVE(IHotkeys);

  public:
  IHotkeys()          = default;
  virtual ~IHotkeys() = default;

  virtual void    registerCallback(HkCommand cmd, HkFunc func)                                             = 0;
  virtual bool    isPressed(HkCommand cmd)                                                                 = 0;
  virtual int32_t isPressedEx(HkCommand cmd1, HkCommand cmd2, int32_t ifcmd1, int32_t ifcmd2, int32_t def) = 0;
  virtual void    update(const SDL_KeyboardEvent* event)                                                   = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL IHotkeys& accessHotkeys();
#undef __APICALL
