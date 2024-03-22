#include "common.h"
#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "logging.h"
#include "types.h"

#include <SDL.h>
#include <algorithm>
#include <bitset>
#include <chrono>

LOG_DEFINE_MODULE(libScePad);

namespace {
struct Controller {
  int32_t userId       = -1;
  uint8_t countConnect = 0;

  SDL_GameController* padPtr;
  ScePadData          prePadData;
};

struct Pimpl {
  Pimpl() = default;
  std::mutex m_mutexInt;

  std::array<Controller, 16 /* Define? */> controller;
};

static auto* getData() {
  static Pimpl obj;
  return &obj;
}

static SDL_GameController* setupPad(int n, int userId) {
  auto pData       = getData();
  auto pController = SDL_GameControllerOpen(n);
  if (pController == nullptr) return nullptr;

  SDL_GameControllerSetPlayerIndex(pController, userId - 1);
  if (userId > 0) pData->controller[n].userId = userId;
  pData->controller[n].prePadData = ScePadData();
  pData->controller[n].padPtr     = pController;
  ++pData->controller[n].countConnect;

  return pController;
}

uint32_t getButtons(SDL_GameController* pad) {
  std::bitset<32> bits;
  bits[(uint32_t)ScePadButtonDataOffset::L3]        = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSTICK);
  bits[(uint32_t)ScePadButtonDataOffset::R3]        = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
  bits[(uint32_t)ScePadButtonDataOffset::OPTIONS]   = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START);
  bits[(uint32_t)ScePadButtonDataOffset::UP]        = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP);
  bits[(uint32_t)ScePadButtonDataOffset::RIGHT]     = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
  bits[(uint32_t)ScePadButtonDataOffset::DOWN]      = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  bits[(uint32_t)ScePadButtonDataOffset::LEFT]      = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  bits[(uint32_t)ScePadButtonDataOffset::L1]        = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
  bits[(uint32_t)ScePadButtonDataOffset::L2]        = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 100;
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  bits[(uint32_t)ScePadButtonDataOffset::R2]        = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 100;
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_Y);
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B);
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A);
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X);
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_TOUCHPAD);
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

ScePadData getPadData(int handle) {
  LOG_USE_MODULE(libScePad);
  ScePadData data;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;

  if (pController == nullptr) return ScePadData();
  auto lockSDL2 = accessVideoOut().getSDLLock();

  data = ScePadData {
      .buttons = getButtons(pController),
      .leftStick =
          {
              .x = scaleAnalogStick(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_LEFTX)),
              .y = scaleAnalogStick(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_LEFTY)),
          },
      .rightStick =
          {
              .x = scaleAnalogStick(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_RIGHTX)),
              .y = scaleAnalogStick(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_RIGHTY)),
          },
      .analogButtons =
          {
              .l2 = scaleAnalogButton(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_TRIGGERLEFT)),
              .r2 = scaleAnalogButton(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)),
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

      .connected           = SDL_GameControllerGetAttached(pController) == true,
      .timestamp           = accessTimer().getTicks(),
      .connectedCount      = pData->controller[handle].countConnect,
      .deviceUniqueDataLen = 0,
  };

  data.touchData.touchNum = 0;
  for (int f = 0; f < SDL_GameControllerGetNumTouchpadFingers(pController, 0); f++) {
    float x = 0.0f, y = 0.0f, p = 0.0f;

    SDL_GameControllerGetTouchpadFinger(pController, 0, f, NULL, &x, &y, &p);
    if (p > 0) {
      auto& touch = data.touchData.touch[data.touchData.touchNum++];
      touch.x = x * 1920, touch.y = y * 950, touch.id = f;
    }
  }

  if (SDL_GameControllerIsSensorEnabled(pController, SDL_SENSOR_GYRO)) {
    float gdata[3];
    if (SDL_GameControllerGetSensorData(pController, SDL_SENSOR_GYRO, gdata, 3) == 0) {
      auto cr = std::cosf(gdata[2] * 0.5f);
      auto sr = std::sinf(gdata[2] * 0.5f);
      auto cp = std::cosf(gdata[0] * 0.5f);
      auto sp = std::sinf(gdata[0] * 0.5f);
      auto cy = std::cosf(gdata[1] * 0.5f);
      auto sy = std::sinf(gdata[1] * 0.5f);

      data.orientation = {
          .x = sr * cp * cy - cr * sp * sy,
          .y = cr * sp * cy + sr * cp * sy,
          .z = cr * cp * sy - sr * sp * cy,
          .w = cr * cp * cy + sr * sp * sy,
      };
    }
  }

  if (SDL_GameControllerIsSensorEnabled(pController, SDL_SENSOR_ACCEL)) {
    float adata[3];
    if (SDL_GameControllerGetSensorData(pController, SDL_SENSOR_ACCEL, adata, 3) == 0) {
      data.acceleration = {.x = adata[0], .y = adata[1], .z = adata[2]};
    }
  }

  return data;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePad";

constexpr uint32_t initflags = SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_SENSOR;

EXPORT SYSV_ABI int scePadInit(void) {
  LOG_USE_MODULE(libScePad);

  int32_t ret = SDL_InitSubSystem(initflags) == 0;
  if (SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0) {
    LOG_WARN(L"Failed to load game controller mappings");
  }

  SDL_JoystickEventState(SDL_ENABLE);
  SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
  SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
  return ret ? Ok : Err::FATAL;
}

EXPORT SYSV_ABI int scePadOpen(int32_t userId, PadPortType type, int32_t index, const void* pParam) {
  LOG_USE_MODULE(libScePad);

  auto pData = getData();

  std::unique_lock const lock(pData->m_mutexInt);

  // Check already opened
  for (size_t n = 0; n < 16; ++n) {
    if (pData->controller[n].userId == userId) return Err::ALREADY_OPENED;
  }
  // - already open

  auto lockSDL2 = accessVideoOut().getSDLLock();
  for (size_t n = 0; n < 16; ++n) {
    if (pData->controller[n].userId >= 0) continue;
    auto pController = setupPad(n, userId);
    char guid[33];

    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(SDL_GameControllerGetJoystick(pController)), guid, 33);
    LOG_INFO(L"-> Pad[%llu]: userId:%d name:%S guid:%S", n, userId, SDL_GameControllerNameForIndex(n), guid);
    return n;
  }

  return Err::NO_HANDLE;
}

EXPORT SYSV_ABI int scePadClose(int32_t handle) {
  if (handle < 0) return Ok;

  LOG_USE_MODULE(libScePad);

  auto pData = getData();

  std::unique_lock const lock(pData->m_mutexInt);
  LOG_INFO(L"<- Pad[%d]", handle);
  pData->controller[handle].userId = -1;
  SDL_GameControllerClose(pData->controller[handle].padPtr);
  pData->controller[handle].padPtr = nullptr;

  return Ok;
}

EXPORT SYSV_ABI int scePadGetHandle(int32_t userId, PadPortType type, int32_t index) {
  auto pData = getData();
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"");
  std::unique_lock const lock(pData->m_mutexInt);

  for (size_t n = 0; n < 16; ++n) {
    if (pData->controller[n].userId == userId) {
      return n;
    }
  }
  return Err::NO_HANDLE;
}

EXPORT SYSV_ABI int scePadRead(int32_t handle, ScePadData* pPadData, int32_t num) {
  LOG_USE_MODULE(libScePad);
  if (handle < 0) return Err::INVALID_HANDLE;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;
  if (SDL_GameControllerGetAttached(pController) == false) {
    SDL_GameControllerClose(pController);
    if ((pController = setupPad(handle, 0)) == nullptr) return Err::DEVICE_NOT_CONNECTED;
  }

  std::unique_lock const lock(pData->m_mutexInt);

  *pPadData = getPadData(handle);

  int retVal = std::memcmp((uint8_t*)&pData->controller[handle].prePadData, (uint8_t*)pPadData, offsetof(ScePadData, connected));
  LOG_TRACE(L"buttons 0x%lx leftStick:%u/%u rightStick:%u/%u L2/R2:%u,%u diff:%d", pPadData->buttons, pPadData->leftStick.x, pPadData->leftStick.y,
            pPadData->rightStick.x, pPadData->rightStick.y, pPadData->analogButtons.l2, pPadData->analogButtons.r2, retVal);

  pData->controller[handle].prePadData = *pPadData;
  return abs(retVal);
}

EXPORT SYSV_ABI int scePadReadState(int32_t handle, ScePadData* pData) {
  auto ret = scePadRead(handle, pData, 1);
  return ret >= 0 ? Ok : ret;
}

EXPORT SYSV_ABI int scePadSetMotionSensorState(int32_t handle, bool bEnable) {
  LOG_USE_MODULE(libScePad);
  if (handle < 0) return Err::INVALID_HANDLE;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;

  if (SDL_GameControllerHasSensor(pController, SDL_SENSOR_GYRO)) {
    if (SDL_GameControllerSetSensorEnabled(pController, SDL_SENSOR_GYRO, (SDL_bool)bEnable) == 0 &&
        SDL_GameControllerSetSensorEnabled(pController, SDL_SENSOR_ACCEL, (SDL_bool)bEnable) == 0) {
      LOG_INFO(L"Gyroscope sensor %S successfully!", bEnable ? "enabled" : "disabled");
    } else {
      LOG_WARN(L"Failed to enable the gyroscope sensor: %S", SDL_GetError());
    }
  }

  return Ok;
}

EXPORT SYSV_ABI int scePadSetTiltCorrectionState(int32_t handle, bool bEnable) {
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI int scePadSetAngularVelocityDeadbandState(int32_t handle, bool bEnable) {
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI int scePadResetOrientation(int32_t handle) {
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI int scePadSetVibration(int32_t handle, const ScePadVibrationParam* pParam) {
  if (handle < 0) return Err::INVALID_HANDLE;
  if (pParam == nullptr) return Err::INVALID_ARG;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;

  SDL_GameControllerRumble(pController, ((float)pParam->smallMotor / 255.0f) * 0xFFFF, ((float)pParam->largeMotor / 255.0f) * 0xFFFF, -1);
  return Ok;
}

EXPORT SYSV_ABI int scePadSetLightBar(int32_t handle, const ScePadColor* pParam) {
  if (handle < 0) return Err::INVALID_HANDLE;
  if (pParam == nullptr) return Err::INVALID_ARG;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;

  if (SDL_GameControllerHasLED(pController) == false) return Err::INVALID_LIGHTBAR_SETTING;

  SDL_GameControllerSetLED(pController, pParam->r, pParam->g, pParam->b);
  return Ok;
}

EXPORT SYSV_ABI int scePadResetLightBar(int32_t handle) {
  if (handle < 0) return Err::INVALID_HANDLE;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;
  if (SDL_GameControllerHasLED(pController) == false) return Err::INVALID_LIGHTBAR_SETTING;

  SDL_GameControllerSetLED(pController, 0, 0, 0);
  return Ok;
}

EXPORT SYSV_ABI int scePadGetControllerInformation(int32_t handle, ScePadControllerInformation* pInfo) {
  LOG_USE_MODULE(libScePad);
  if (handle < 0) return Err::INVALID_HANDLE;
  auto pData = getData();

  std::unique_lock const lock(pData->m_mutexInt);

  auto const& pad = pData->controller[handle];

  // Not connected -> defaults
  if (pad.userId < 0) {
    pInfo->touchPadInfo.pixelDensity = 1.f;
    pInfo->touchPadInfo.resolution.x = 1920;
    pInfo->touchPadInfo.resolution.y = 950;

    pInfo->stickInfo.deadZoneLeft  = 2; // todo make config
    pInfo->stickInfo.deadZoneRight = 2; // todo make config
    pInfo->connectionType          = (uint8_t)PadPortType::STANDARD;
    pInfo->connectedCount          = pad.countConnect;
    pInfo->connected               = false;
    pInfo->deviceClass             = ScePadDeviceClass::STANDARD;
    return Ok;
  }
  // -

  pInfo->touchPadInfo.pixelDensity = 44.86f;
  pInfo->touchPadInfo.resolution.x = 1920;
  pInfo->touchPadInfo.resolution.y = 943;

  pInfo->stickInfo.deadZoneLeft  = 2; // todo make config
  pInfo->stickInfo.deadZoneRight = 2; // todo make config

  auto lockSDL2         = accessVideoOut().getSDLLock();
  pInfo->connectionType = (uint8_t)PadPortType::STANDARD;
  pInfo->connectedCount = pad.countConnect;
  pInfo->connected      = SDL_GameControllerGetAttached(pad.padPtr) == true;
  pInfo->deviceClass    = ScePadDeviceClass::STANDARD;

  LOG_DEBUG(L"handle:%d connected:%d", handle, pInfo->connected);
  return Ok;
}

EXPORT SYSV_ABI int scePadDeviceClassParseData(int32_t handle, const ScePadData* pData, ScePadDeviceClassData* pDeviceClassData) {
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI int scePadDeviceClassGetExtendedInformation(int32_t handle, ScePadDeviceClassExtendedInformation* pExtInfo) {
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI void scePadTerminate() {
  SDL_QuitSubSystem(initflags);
}
}
