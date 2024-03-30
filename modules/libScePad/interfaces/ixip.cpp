#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "ikbd.h"
#include "logging.h"

#include <Windows.h>
#include <Xinput.h>
#include <bitset>
#include <math.h>

LOG_DEFINE_MODULE(libScePad_xip);

typedef void (*XInputEnableProc)(BOOL);
typedef DWORD (*XInputGetStateProc)(DWORD, XINPUT_STATE*);
typedef DWORD (*XInputSetStateProc)(DWORD, XINPUT_VIBRATION*);
typedef DWORD (*XInputGetCapabilitiesProc)(DWORD, DWORD, XINPUT_CAPABILITIES*);

static HMODULE                   xip_lib          = nullptr;
static XInputEnableProc          xip_enableFunc   = nullptr;
static XInputGetStateProc        xip_getStateFunc = nullptr;
static XInputSetStateProc        xip_setStateFunc = nullptr;
static XInputGetCapabilitiesProc xip_getCapsFunc  = nullptr;

class XIPController: public IController {
  DWORD m_xUserId         = -1;
  bool  m_xRumblePossible = false;

  public:
  XIPController(ControllerConfig* cfg, uint32_t userid): IController(ControllerType::SDL, cfg, userid) {
    init();
    reconnect();
  }

  virtual ~XIPController() = default;

  // ### Interface
  bool reconnect() final;
  void close() final;
  bool readPadData(ScePadData& data) final;
  bool setMotion(bool state) final;
  bool resetOrientation() final;
  bool setRumble(const ScePadVibrationParam* pParam) final;
  bool setLED(const ScePadColor* pParam) final;
  bool resetLED() final;

  uint32_t getButtons(XINPUT_GAMEPAD* xgp);
  void     init();
};

std::unique_ptr<IController> createController_xinput(ControllerConfig* cfg, uint32_t userid) {
  return std::make_unique<XIPController>(cfg, userid);
}

void XIPController::init() {
  static std::once_flag init;

  std::call_once(init, [] {
    LOG_USE_MODULE(libScePad_xip);

    auto initFunc = [] {
      if ((xip_lib = LoadLibrary(XINPUT_DLL_A)) == nullptr) return false;
      if ((xip_enableFunc = (XInputEnableProc)GetProcAddress(xip_lib, "XInputEnable")) == nullptr) return false;
      if ((xip_getStateFunc = (XInputGetStateProc)GetProcAddress(xip_lib, "XInputGetState")) == nullptr) return false;
      if ((xip_setStateFunc = (XInputSetStateProc)GetProcAddress(xip_lib, "XInputSetState")) == nullptr) return false;
      if ((xip_getCapsFunc = (XInputGetCapabilitiesProc)GetProcAddress(xip_lib, "XInputGetCapabilities")) == nullptr) return false;
      xip_enableFunc(TRUE);
      return true;
    };

    if (!initFunc()) {
      LOG_CRIT(L"Failed to load XInput: %d", GetLastError());
    }
  });
}

void XIPController::close() {
  if (m_state == ControllerState::Disconnected || m_state == ControllerState::Closed) return;
  m_state = ControllerState::Closed;
}

bool XIPController::reconnect() {
  LOG_USE_MODULE(libScePad_xip);

  XINPUT_CAPABILITIES caps;
  for (DWORD n = 0; n < XUSER_MAX_COUNT; n++) {
    if (xip_getCapsFunc(n, XINPUT_FLAG_GAMEPAD, &caps) != ERROR_SUCCESS) continue;
    if (caps.Type != XINPUT_DEVTYPE_GAMEPAD || caps.SubType != XINPUT_DEVSUBTYPE_GAMEPAD) continue;
    ::strcpy_s(m_name, "XInput gamepad");
    ::strcpy_s(m_guid, "1337deadbeef00000000000000000000");
    m_xRumblePossible = caps.Vibration.wLeftMotorSpeed > 0 || caps.Vibration.wRightMotorSpeed > 0;
    if (caps.Flags & XINPUT_CAPS_NO_NAVIGATION) {
      LOG_WARN(L"Your gamepad lacks menu navigation buttons, you may not be able to reach some parts of game menus!");
    }
    m_xUserId = n;
    return true;
  }

  m_xUserId = -1;
  return false;
}

uint32_t XIPController::getButtons(XINPUT_GAMEPAD* xgp) {
  if (m_xUserId == -1) return 0;

  std::bitset<32> bits;
  bits[(uint32_t)ScePadButtonDataOffset::L3]        = xgp->wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
  bits[(uint32_t)ScePadButtonDataOffset::R3]        = xgp->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
  bits[(uint32_t)ScePadButtonDataOffset::OPTIONS]   = xgp->wButtons & XINPUT_GAMEPAD_START;
  bits[(uint32_t)ScePadButtonDataOffset::UP]        = xgp->wButtons & XINPUT_GAMEPAD_DPAD_UP;
  bits[(uint32_t)ScePadButtonDataOffset::RIGHT]     = xgp->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
  bits[(uint32_t)ScePadButtonDataOffset::DOWN]      = xgp->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
  bits[(uint32_t)ScePadButtonDataOffset::LEFT]      = xgp->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
  bits[(uint32_t)ScePadButtonDataOffset::L1]        = xgp->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
  bits[(uint32_t)ScePadButtonDataOffset::L2]        = xgp->bLeftTrigger > 0;
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = xgp->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
  bits[(uint32_t)ScePadButtonDataOffset::R2]        = xgp->bRightTrigger > 0;
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = xgp->wButtons & XINPUT_GAMEPAD_Y;
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = xgp->wButtons & XINPUT_GAMEPAD_B;
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = xgp->wButtons & XINPUT_GAMEPAD_A;
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = xgp->wButtons & XINPUT_GAMEPAD_X;
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = xgp->wButtons & XINPUT_GAMEPAD_BACK;

  return bits.to_ulong();
}

bool XIPController::readPadData(ScePadData& data) {
  auto lockSDL2 = accessVideoOut().getSDLLock();

  if (m_state == ControllerState::Closed) return false;

  XINPUT_STATE xstate;
  if (xip_getStateFunc(m_xUserId, &xstate) != ERROR_SUCCESS) {
    m_state = ControllerState::Disconnected;
    data    = ScePadData {};
    return false;
  }

  auto xGamepad = &xstate.Gamepad;

  data = ScePadData {
      .buttons = getButtons(xGamepad),
      .leftStick =
          {
              .x = scaleAnalogStick(xGamepad->sThumbLX),
              .y = scaleAnalogStick(-xGamepad->sThumbLY - 1 /* prevent short overflow */),
          },
      .rightStick =
          {
              .x = scaleAnalogStick(xGamepad->sThumbRX),
              .y = scaleAnalogStick(-xGamepad->sThumbRY - 1 /* prevent short overflow */),
          },
      .analogButtons =
          {
              .l2 = xGamepad->bLeftTrigger,
              .r2 = xGamepad->bRightTrigger,
          },
      .orientation =
          {
              .x = 0.0f,
              .y = 0.0f,
              .z = 0.0f,
              .w = 1.0f,
          },
      .acceleration =
          {
              .x = 0.0f,
              .y = 0.0f,
              .z = 0.0f,
          },
      .angularVelocity =
          {
              .x = 0.0f,
              .y = 0.0f,
              .z = 0.0f,
          },
      .touchData =
          {
              .touchNum = 0,
              .touch    = {{
                               .x  = 0,
                               .y  = 0,
                               .id = 1,
                        },
                           {
                               .x  = 0,
                               .y  = 0,
                               .id = 2,
                        }},

          },

      .connected           = true,
      .timestamp           = accessTimer().getTicks(),
      .connectedCount      = m_connectCount,
      .deviceUniqueDataLen = 0,
  };

  return true;
}

bool XIPController::setMotion(bool state) {
  return false;
}

bool XIPController::resetOrientation() {
  return false;
}

bool XIPController::setRumble(const ScePadVibrationParam* pParam) {
  if (!m_xRumblePossible) return false;

  // todo: Handle case with one-motor gamepad. Is that even possible?
  XINPUT_VIBRATION vibe {.wLeftMotorSpeed  = static_cast<WORD>((WORD)pParam->smallMotor * 257),
                         .wRightMotorSpeed = static_cast<WORD>((WORD)pParam->largeMotor * 257)};
  return xip_setStateFunc(m_xUserId, &vibe) == ERROR_SUCCESS;
}

bool XIPController::setLED(const ScePadColor* pParam) {
  m_lastColor = *pParam;
  return false;
}

bool XIPController::resetLED() {
  m_lastColor = {0x00, 0x00, 0xff};
  return setLED(&m_lastColor);
}
