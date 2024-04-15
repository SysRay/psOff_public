#include "ikbd.h"

#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
// #include "logging.h"

#include <SDL2/SDL.h>
#include <bitset>
#include <math.h>

// LOG_DEFINE_MODULE(libScePad_kbd);

class KBDController: public IController {
  public:
  KBDController(ControllerConfig* cfg, uint32_t userid): IController(ControllerType::SDL, cfg, userid) { reconnect(); }

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

  bool     resolveBindFor(const uint8_t* keys, ControllerKey key);
  uint32_t getButtons(const uint8_t* keys);
};

std::unique_ptr<IController> createController_keyboard(ControllerConfig* cfg, uint32_t userid) {
  return std::make_unique<KBDController>(cfg, userid);
}

void KBDController::close() {
  if (m_state == ControllerState::Disconnected || m_state == ControllerState::Closed) return;
  m_state = ControllerState::Closed;
}

bool KBDController::reconnect() {
  m_state = ControllerState::Connected;
  return true;
}

bool KBDController::resolveBindFor(const uint8_t* keys, ControllerKey key) {
  auto bind = m_cfg->GetBind(key);
  return keys[bind.key] && (bind.mod != 0 ? keys[bind.mod] : 1);
}

uint32_t KBDController::getButtons(const uint8_t* keys) {
  std::bitset<32> bits;

  bits[(uint32_t)ScePadButtonDataOffset::L3]        = resolveBindFor(keys, ControllerKey::L3);
  bits[(uint32_t)ScePadButtonDataOffset::R3]        = resolveBindFor(keys, ControllerKey::R3);
  bits[(uint32_t)ScePadButtonDataOffset::OPTIONS]   = resolveBindFor(keys, ControllerKey::OPTIONS);
  bits[(uint32_t)ScePadButtonDataOffset::UP]        = resolveBindFor(keys, ControllerKey::DPAD_UP);
  bits[(uint32_t)ScePadButtonDataOffset::RIGHT]     = resolveBindFor(keys, ControllerKey::DPAD_RIGHT);
  bits[(uint32_t)ScePadButtonDataOffset::DOWN]      = resolveBindFor(keys, ControllerKey::DPAD_DOWN);
  bits[(uint32_t)ScePadButtonDataOffset::LEFT]      = resolveBindFor(keys, ControllerKey::DPAD_LEFT);
  bits[(uint32_t)ScePadButtonDataOffset::L1]        = resolveBindFor(keys, ControllerKey::L1);
  bits[(uint32_t)ScePadButtonDataOffset::L2]        = resolveBindFor(keys, ControllerKey::L2);
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = resolveBindFor(keys, ControllerKey::R1);
  bits[(uint32_t)ScePadButtonDataOffset::R2]        = resolveBindFor(keys, ControllerKey::R2);
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = resolveBindFor(keys, ControllerKey::TRIANGLE);
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = resolveBindFor(keys, ControllerKey::CIRCLE);
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = resolveBindFor(keys, ControllerKey::CROSS);
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = resolveBindFor(keys, ControllerKey::SQUARE);
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = resolveBindFor(keys, ControllerKey::TOUCH_PAD);

  return bits.to_ulong();
}

#define MAP_ANALOG(_keys, _up, _down) (uint8_t)(resolveBindFor(_keys, _up) ? 0xFF : resolveBindFor(_keys, _down) ? 0x00 : 0x7F)

#define MAP_TRIGGER(_keys, _down) (uint8_t)(resolveBindFor(_keys, _down) ? 0xFF : 0x00)

bool KBDController::readPadData(ScePadData& data) {

  if (m_state == ControllerState::Closed) return false;

  const uint8_t* keys = SDL_GetKeyboardState(nullptr);

  data = ScePadData {
      .buttons = getButtons(keys),
      .leftStick =
          {
              .x = MAP_ANALOG(keys, ControllerKey::LX_UP, ControllerKey::LX_DOWN),
              .y = MAP_ANALOG(keys, ControllerKey::LY_UP, ControllerKey::LY_DOWN),
          },
      .rightStick =
          {
              .x = MAP_ANALOG(keys, ControllerKey::RX_UP, ControllerKey::RX_DOWN),
              .y = MAP_ANALOG(keys, ControllerKey::RY_UP, ControllerKey::RY_DOWN),
          },
      .analogButtons =
          {
              .l2 = MAP_TRIGGER(keys, ControllerKey::L2),
              .r2 = MAP_TRIGGER(keys, ControllerKey::R2),
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
