#include "isdl.h"

#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "logging.h"

#include <SDL.h>
#include <bitset>
#include <math.h>

LOG_DEFINE_MODULE(libScePad_sdl);

static bool is_SDL_inited = false;

class SDLController: public IController {
  static constexpr uint32_t m_initflags = SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_SENSOR;
  SDL_GameController*       m_padPtr    = nullptr;

  public:
  SDLController(uint32_t userid): IController(ControllerType::SDL, userid) {
    init();
    m_userId = userid;
    reconnect();
  }

  virtual ~SDLController() { deinit(); }

  // ### Interface
  bool reconnect() final;
  void close() final;
  bool readPadData(ScePadData& data) final;
  bool setMotion(bool state) final;
  bool setRumble(const ScePadVibrationParam* pParam) final;
  bool setLED(const ScePadColor* pParam) final;
  bool resetLED() final;

  uint32_t getButtons();

  static void init();
  static void deinit();
};

std::unique_ptr<IController> createController_sdl(uint32_t userid) {
  return std::make_unique<SDLController>(userid);
}

void SDLController::init() {
  LOG_USE_MODULE(libScePad_sdl);

  if (is_SDL_inited == false) {
    if (SDL_InitSubSystem(m_initflags) != 0) {
      LOG_ERR(L"Failed to initialize SDL subsystem!");
      return;
    }

    if (SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0) {
      LOG_WARN(L"Failed to load game controller mappings");
    }

    SDL_JoystickEventState(SDL_ENABLE);
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
    is_SDL_inited = true;
  }
}

void SDLController::deinit() {
  if (is_SDL_inited == true) {
    // do not uncomment, crashes for some reason
    // looks like this destructor being called after SDL_Quit
    // SDL_QuitSubSystem(m_initflags);
    is_SDL_inited = false;
  }
}

void SDLController::close() {
  if (m_state == ControllerState::Disconnected || m_state == ControllerState::Closed) return;
  if (m_padPtr != nullptr) SDL_GameControllerClose(m_padPtr);
  m_state  = ControllerState::Closed;
  m_padPtr = nullptr;
}

bool SDLController::reconnect() {
  close();

  for (int n = 0; n < SDL_NumJoysticks(); n++) {
    m_padPtr = SDL_GameControllerOpen(n);
    if (m_padPtr == nullptr) continue;

    ++m_connectCount;
    m_state = ControllerState::Connected;
    ::strcpy_s(m_name, SDL_GameControllerName(m_padPtr));
    SDL_GameControllerSetPlayerIndex(m_padPtr, m_userId - 1);
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(SDL_GameControllerGetJoystick(m_padPtr)), m_guid, sizeof(m_guid));
    setLED(&m_lastColor); // restore last known LED color
    return true;
  }

  return false;
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
  bits[(uint32_t)ScePadButtonDataOffset::L2]        = SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 100;
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  bits[(uint32_t)ScePadButtonDataOffset::R2]        = SDL_GameControllerGetAxis(m_padPtr, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 100;
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_Y);
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_B);
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_A);
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_X);
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = SDL_GameControllerGetButton(m_padPtr, SDL_CONTROLLER_BUTTON_TOUCHPAD);

  return bits.to_ulong();
}

uint8_t scaleAnalogStick(Sint16 value) {
  // input -32768 (up/left) to 32767 (down/right).
  // return 0..255

  return 255 * (32768 + value) / (32768 + 32767);
}

uint8_t scaleAnalogButton(Sint16 value) {
  // input 0 when released to 32767
  // return 0..255

  return 255 * value / 32767;
}

bool SDLController::readPadData(ScePadData& data) {
  auto lockSDL2 = accessVideoOut().getSDLLock();

  if (m_state == ControllerState::Closed) return false;

  if (SDL_GameControllerGetAttached(m_padPtr) == false) {
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

      .connected           = SDL_GameControllerGetAttached(m_padPtr) == true,
      .timestamp           = accessTimer().getTicks(),
      .connectedCount      = m_connectCount,
      .deviceUniqueDataLen = 0,
  };

  data.touchData.touchNum = 0;
  for (int f = 0; f < SDL_GameControllerGetNumTouchpadFingers(m_padPtr, 0); f++) {
    float x = 0.0f, y = 0.0f, p = 0.0f;

    SDL_GameControllerGetTouchpadFinger(m_padPtr, 0, f, NULL, &x, &y, &p);
    if (p > 0) {
      auto& touch = data.touchData.touch[data.touchData.touchNum++];
      touch.x = x * 1920, touch.y = y * 950, touch.id = f;
    }
  }

  if (SDL_GameControllerIsSensorEnabled(m_padPtr, SDL_SENSOR_GYRO)) {
    float gdata[3];
    if (SDL_GameControllerGetSensorData(m_padPtr, SDL_SENSOR_GYRO, gdata, 3) == 0) {
      auto cr = ::cosf(gdata[2] * 0.5f);
      auto sr = ::sinf(gdata[2] * 0.5f);
      auto cp = ::cosf(gdata[0] * 0.5f);
      auto sp = ::sinf(gdata[0] * 0.5f);
      auto cy = ::cosf(gdata[1] * 0.5f);
      auto sy = ::sinf(gdata[1] * 0.5f);

      data.orientation = {
          .x = sr * cp * cy - cr * sp * sy,
          .y = cr * sp * cy + sr * cp * sy,
          .z = cr * cp * sy - sr * sp * cy,
          .w = cr * cp * cy + sr * sp * sy,
      };
    }
  }

  if (SDL_GameControllerIsSensorEnabled(m_padPtr, SDL_SENSOR_ACCEL)) {
    float adata[3];
    if (SDL_GameControllerGetSensorData(m_padPtr, SDL_SENSOR_ACCEL, adata, 3) == 0) {
      data.acceleration = {.x = adata[0], .y = adata[1], .z = adata[2]};
    }
  }

  return true;
}

bool SDLController::setMotion(bool state) {
  LOG_USE_MODULE(libScePad_sdl);

  if (SDL_GameControllerHasSensor(m_padPtr, SDL_SENSOR_GYRO)) {
    if (SDL_GameControllerSetSensorEnabled(m_padPtr, SDL_SENSOR_GYRO, (SDL_bool)state) == 0 &&
        SDL_GameControllerSetSensorEnabled(m_padPtr, SDL_SENSOR_ACCEL, (SDL_bool)state) == 0) {
      LOG_INFO(L"Gyroscope sensor %S successfully!", state ? "enabled" : "disabled");
    } else {
      LOG_WARN(L"Failed to enable the gyroscope sensor: %S", SDL_GetError());
    }
  }

  return true;
}

bool SDLController::setRumble(const ScePadVibrationParam* pParam) {
  SDL_GameControllerRumble(m_padPtr, ((float)pParam->smallMotor / 255.0f) * 0xFFFF, ((float)pParam->largeMotor / 255.0f) * 0xFFFF, -1);
  return true;
}

bool SDLController::setLED(const ScePadColor* pParam) {
  if (SDL_GameControllerHasLED(m_padPtr) == false) return false;
  SDL_GameControllerSetLED(m_padPtr, pParam->r, pParam->g, pParam->b);
  m_lastColor = *pParam;
  return true;
}

bool SDLController::resetLED() {
  m_lastColor = {0x00, 0x00, 0xff};
  return setLED(&m_lastColor);
}
