#include "ikbd.h"

#include "internal/hotkeys/hotkeys.h"
#include "core/timer/timer.h"
#include "internal/videoout/videoout.h"
// #include "logging.h"

#include <SDL2/SDL.h>
#include <bitset>
#include <math.h>

// LOG_DEFINE_MODULE(libScePad_kbd);

class KBDController: public IController {
  public:
  KBDController(uint32_t userid): IController(ControllerType::SDL, userid) { reconnect(); }

  virtual ~KBDController() = default;

  // ### Interface
  bool reconnect() final;
  void close() final;
  bool readPadData(ScePadData& data) final;
  bool setMotion(bool state) final;
  bool resetOrientation() final;
  bool setRumble(const ScePadVibrationParam* pParam) final;
  bool setLED(const ScePadColor* pParam) final;
  bool resetLED() final;

  uint32_t getButtons(IHotkeys& keys);
};

std::unique_ptr<IController> createController_keyboard(uint32_t userid) {
  return std::make_unique<KBDController>(userid);
}

void KBDController::close() {
  if (m_state == ControllerState::Disconnected || m_state == ControllerState::Closed) return;
  m_state = ControllerState::Closed;
}

bool KBDController::reconnect() {
  m_state = ControllerState::Connected;
  ::strcpy_s(m_name, "Emulated gamepad");
  ::strcpy_s(m_guid, "1337deadbeefb00b1e50101010101010");
  return true;
}

uint32_t KBDController::getButtons(IHotkeys& hk) {
  std::bitset<32> bits;

  bits[(uint32_t)ScePadButtonDataOffset::L3]        = hk.isPressed(HkCommand::CONTROLLER_L3);
  bits[(uint32_t)ScePadButtonDataOffset::R3]        = hk.isPressed(HkCommand::CONTROLLER_R3);
  bits[(uint32_t)ScePadButtonDataOffset::OPTIONS]   = hk.isPressed(HkCommand::CONTROLLER_OPTIONS);
  bits[(uint32_t)ScePadButtonDataOffset::UP]        = hk.isPressed(HkCommand::CONTROLLER_DPAD_UP);
  bits[(uint32_t)ScePadButtonDataOffset::RIGHT]     = hk.isPressed(HkCommand::CONTROLLER_DPAD_RIGHT);
  bits[(uint32_t)ScePadButtonDataOffset::DOWN]      = hk.isPressed(HkCommand::CONTROLLER_DPAD_DOWN);
  bits[(uint32_t)ScePadButtonDataOffset::LEFT]      = hk.isPressed(HkCommand::CONTROLLER_DPAD_LEFT);
  bits[(uint32_t)ScePadButtonDataOffset::L1]        = hk.isPressed(HkCommand::CONTROLLER_L1);
  bits[(uint32_t)ScePadButtonDataOffset::L2]        = hk.isPressed(HkCommand::CONTROLLER_L2);
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = hk.isPressed(HkCommand::CONTROLLER_R1);
  bits[(uint32_t)ScePadButtonDataOffset::R2]        = hk.isPressed(HkCommand::CONTROLLER_R2);
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = hk.isPressed(HkCommand::CONTROLLER_TRIANGLE);
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = hk.isPressed(HkCommand::CONTROLLER_CIRCLE);
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = hk.isPressed(HkCommand::CONTROLLER_CROSS);
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = hk.isPressed(HkCommand::CONTROLLER_SQUARE);
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = hk.isPressed(HkCommand::CONTROLLER_TOUCH_PAD);

  return bits.to_ulong();
}

bool KBDController::readPadData(ScePadData& data) {
  if (m_state == ControllerState::Closed) return false;

  auto& hk = accessHotkeys();

  data = ScePadData {
      .buttons = getButtons(hk),
      .leftStick =
          {
              .x = uint8_t(hk.isPressedEx(HkCommand::CONTROLLER_LX_UP, HkCommand::CONTROLLER_LX_DOWN, 0xFF, 0x00, 0x7F)),
              .y = uint8_t(hk.isPressedEx(HkCommand::CONTROLLER_LY_UP, HkCommand::CONTROLLER_LY_DOWN, 0xFF, 0x00, 0x7F)),
          },
      .rightStick =
          {
              .x = uint8_t(hk.isPressedEx(HkCommand::CONTROLLER_RX_UP, HkCommand::CONTROLLER_RX_DOWN, 0xFF, 0x00, 0x7F)),
              .y = uint8_t(hk.isPressedEx(HkCommand::CONTROLLER_RY_UP, HkCommand::CONTROLLER_RY_DOWN, 0xFF, 0x00, 0x7F)),
          },
      .analogButtons =
          {
              .l2 = uint8_t(hk.isPressed(HkCommand::CONTROLLER_L2) ? 0xFF : 0x00),
              .r2 = uint8_t(hk.isPressed(HkCommand::CONTROLLER_R2) ? 0xFF : 0x00),
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

bool KBDController::setMotion(bool state) {
  return false;
}

bool KBDController::resetOrientation() {
  return false;
}

bool KBDController::setRumble(const ScePadVibrationParam* pParam) {
  return false;
}

bool KBDController::setLED(const ScePadColor* pParam) {
  m_lastColor = *pParam;
  return false;
}

bool KBDController::resetLED() {
  m_lastColor = {0x00, 0x00, 0xff};
  return setLED(&m_lastColor);
}
