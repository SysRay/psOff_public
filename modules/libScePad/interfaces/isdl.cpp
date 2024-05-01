#include "isdl.h"

#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "logging.h"

#include <SDL2/SDL.h>
#include <bitset>
#include <math.h>

LOG_DEFINE_MODULE(libScePad_sdl);

class SDLPadManager {
  std::vector<SDL_GameController*> m_openedPads;

  public:
  SDL_GameController* openNew() {
    for (int n = 0; n < SDL_NumJoysticks(); n++) {
      if (!SDL_IsGameController(n)) continue;
      auto pad = SDL_GameControllerOpen(n);
      if (std::find(m_openedPads.begin(), m_openedPads.end(), pad) != m_openedPads.end()) continue;
      m_openedPads.push_back(pad);
      return pad;
    }

    return nullptr;
  }

  void close(SDL_GameController* pad) {
    auto it = std::find(m_openedPads.begin(), m_openedPads.end(), pad);
    if (it == m_openedPads.end()) return;
    SDL_GameControllerClose(pad);
    m_openedPads.erase(it);
  }
};

static SDLPadManager& getPadManager() {
  static SDLPadManager im;
  return im;
}

class SDLController: public IController {
  static constexpr uint32_t m_initflags     = SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_SENSOR;
  SDL_GameController*       m_padPtr        = nullptr;
  bool                      m_motionEnabled = false;
  uint64_t                  m_lastTimeStamp = 0;
  SceFVector3               m_gyroData      = {0.0f, 0.0f, 0.0f};

  public:
  SDLController(ControllerConfig* cfg, uint32_t userid): IController(ControllerType::SDL, cfg, userid) {
    init();
    reconnect();
  }

  virtual ~SDLController() = default;

  // ### Interface
  bool reconnect() final;
  void close() final;
  bool readPadData(ScePadData& data) final;
  bool setMotion(bool state) final;
  bool resetOrientation() final;
  bool setRumble(const ScePadVibrationParam* pParam) final;
  bool setLED(const ScePadColor* pParam) final;
  bool resetLED() final;

  uint32_t getButtons();

  static void init();
};

std::unique_ptr<IController> createController_sdl(ControllerConfig* cfg, uint32_t userid) {
  return std::make_unique<SDLController>(cfg, userid);
}

void SDLController::init() {
  static std::once_flag init;

  std::call_once(init, [] {
    LOG_USE_MODULE(libScePad_sdl);

    if (accessVideoOut().SDLInit(m_initflags) != 0) {
      return;
    }

    if (SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0) {
      LOG_WARN(L"Failed to load game controller mappings: %S", SDL_GetError());
    }

    SDL_JoystickEventState(SDL_ENABLE);
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
  });
}

void SDLController::close() {
  if (m_state == ControllerState::Disconnected || m_state == ControllerState::Closed) return;
  if (m_padPtr != nullptr) getPadManager().close(m_padPtr);
  m_state  = ControllerState::Closed;
  m_padPtr = nullptr;
}

bool SDLController::reconnect() {
  close();

  m_padPtr = getPadManager().openNew();
  if (m_padPtr == nullptr) return false;

  ++m_connectCount;
  m_state = ControllerState::Connected;
  ::strcpy_s(m_name, SDL_GameControllerName(m_padPtr));
  SDL_GameControllerSetPlayerIndex(m_padPtr, m_userId - 1);
  SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(SDL_GameControllerGetJoystick(m_padPtr)), m_guid, sizeof(m_guid));
  setLED(&m_lastColor); // restore last known LED color
  setMotion(m_motionEnabled);

  return true;
}

uint32_t SDLController::getButtons() {
  if (m_padPtr == nullptr) return 0;

  std::bitset<32> bits;
  bits[(uint32_t)ScePadButtonDataOffset::L3]        = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_LEFTSTICK);
  bits[(uint32_t)ScePadButtonDataOffset::R3]        = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
  bits[(uint32_t)ScePadButtonDataOffset::OPTIONS]   = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_START);
  bits[(uint32_t)ScePadButtonDataOffset::UP]        = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_DPAD_UP);
  bits[(uint32_t)ScePadButtonDataOffset::RIGHT]     = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
  bits[(uint32_t)ScePadButtonDataOffset::DOWN]      = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  bits[(uint32_t)ScePadButtonDataOffset::LEFT]      = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  bits[(uint32_t)ScePadButtonDataOffset::L1]        = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
  bits[(uint32_t)ScePadButtonDataOffset::L2]        = SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 0;
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  bits[(uint32_t)ScePadButtonDataOffset::R2]        = SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 0;
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_Y);
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_B);
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_A);
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_X);
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_TOUCHPAD);

  return bits.to_ulong();
}

bool SDLController::readPadData(ScePadData& data) {

  if (m_state == ControllerState::Closed) return false;

  if (SDL_GameControllerGetAttached(m_padPtr) == SDL_FALSE) {
    m_state = ControllerState::Disconnected;
    if (!reconnect()) {
      data = ScePadData {};
      return false;
    }
  }

  data = ScePadData {
      .buttons = getButtons(),
      .leftStick =
          {
              .x = scaleAnalogStick(SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_LEFTX)),
              .y = scaleAnalogStick(SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_LEFTY)),
          },
      .rightStick =
          {
              .x = scaleAnalogStick(SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_RIGHTX)),
              .y = scaleAnalogStick(SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_RIGHTY)),
          },
      .analogButtons =
          {
              .l2 = scaleAnalogButton(SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_TRIGGERLEFT)),
              .r2 = scaleAnalogButton(SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)),
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
              .touch =
                  {
                      {
                          .x  = 0,
                          .y  = 0,
                          .id = 1,
                      },
                      {
                          .x  = 0,
                          .y  = 0,
                          .id = 2,
                      },
                  },

          },

      .connected           = IController::isConnected(),
      .timestamp           = accessTimer().getTicks(),
      .connectedCount      = m_connectCount,
      .deviceUniqueDataLen = 0,
  };

  data.touchData.touchNum = 0;
  for (int f = 0; f < SDL_GameControllerGetNumTouchpadFingers(m_padPtr, 0); f++) {
    if (data.touchData.touchNum == SCE_PAD_MAX_TOUCH_NUM) break;
    float x = 0.0f, y = 0.0f, p = 0.0f;

    SDL_GameControllerGetTouchpadFinger(m_padPtr, 0, f, NULL, &x, &y, &p);
    if (p > 0) {
      auto& touch = data.touchData.touch[data.touchData.touchNum++];
      touch.x = x * 1920, touch.y = y * 950, touch.id = f;
    }
  }

  if (SDL_GameControllerIsSensorEnabled(m_padPtr, SDL_SENSOR_GYRO) == SDL_TRUE) { // todo: fix this
    float    gdata[3];
    uint64_t timestamp, tdiff;
    if (SDL_GameControllerGetSensorDataWithTimestamp(m_padPtr, SDL_SENSOR_GYRO, &timestamp, gdata, 3) == 0 && timestamp != m_lastTimeStamp) {
      tdiff           = timestamp - m_lastTimeStamp;
      m_lastTimeStamp = timestamp;

      m_gyroData.x += gdata[0] / tdiff;
      m_gyroData.y += gdata[1] / tdiff;
      m_gyroData.z += gdata[2] / tdiff;
    }

    auto cr = ::cosf(m_gyroData.x * 0.5f);
    auto sr = ::sinf(m_gyroData.x * 0.5f);
    auto cp = ::cosf(m_gyroData.y * 0.5f);
    auto sp = ::sinf(m_gyroData.y * 0.5f);
    auto cy = ::cosf(m_gyroData.z * 0.5f);
    auto sy = ::sinf(m_gyroData.z * 0.5f);

    data.orientation = {
        .x = sr * cp * cy - cr * sp * sy,
        .y = cr * sp * cy + sr * cp * sy,
        .z = cr * cp * sy - sr * sp * cy,
        .w = cr * cp * cy + sr * sp * sy,
    };
  }

  if (SDL_GameControllerIsSensorEnabled(m_padPtr, SDL_SENSOR_ACCEL) == SDL_TRUE) {
    float adata[3];
    if (SDL_GameControllerGetSensorData(m_padPtr, SDL_SENSOR_ACCEL, adata, 3) == 0) {
      data.acceleration = {.x = adata[0], .y = adata[1], .z = adata[2]};
    }
  }

  return true;
}

bool SDLController::setMotion(bool state) {
  LOG_USE_MODULE(libScePad_sdl);
  m_motionEnabled = state;

  if (SDL_GameControllerHasSensor(m_padPtr, SDL_SENSOR_GYRO) == SDL_TRUE) {
    if (SDL_GameControllerSetSensorEnabled(m_padPtr, SDL_SENSOR_GYRO, (SDL_bool)state) == 0 &&
        SDL_GameControllerSetSensorEnabled(m_padPtr, SDL_SENSOR_ACCEL, (SDL_bool)state) == 0) {
      LOG_INFO(L"Gyroscope sensor %S successfully!", state ? "enabled" : "disabled");
      return true;
    } else {
      LOG_WARN(L"Failed to enable the gyroscope sensor: %S", SDL_GetError());
    }
  }

  return false;
}

bool SDLController::resetOrientation() {
  m_gyroData = {0.0f, 0.0f, 0.0f};
  return true;
}

bool SDLController::setRumble(const ScePadVibrationParam* pParam) {
  return SDL_GameControllerRumble(m_padPtr, ((float)pParam->smallMotor / 255.0f) * 0xFFFF, ((float)pParam->largeMotor / 255.0f) * 0xFFFF, -1) == 0;
}

bool SDLController::setLED(const ScePadColor* pParam) {
  if (SDL_GameControllerHasLED(m_padPtr) == SDL_FALSE) return false;
  SDL_GameControllerSetLED(m_padPtr, pParam->r, pParam->g, pParam->b);
  m_lastColor = *pParam;
  return true;
}

bool SDLController::resetLED() {
  m_lastColor = {0x00, 0x00, 0xff};
  return setLED(&m_lastColor);
}
