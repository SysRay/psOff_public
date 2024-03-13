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
  auto pData  = getData();
  auto padPtr = SDL_GameControllerOpen(n);
  if (padPtr == nullptr) return nullptr;

  SDL_GameControllerSetPlayerIndex(padPtr, userId - 1);
  if (userId > 0) pData->controller[n].userId = userId;
  pData->controller[n].prePadData = ScePadData();
  pData->controller[n].padPtr     = padPtr;
  ++pData->controller[n].countConnect;
  return padPtr;
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
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_Y);
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B);
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A);
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X);
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_TOUCHPAD);
  return bits.to_ulong();
}

uint8_t convertAnalog(float value) {
  // todo: windows uses 255 ps4 256
  int32_t v = (256.0f + 255.0f * value) / 2.0f;
  // int32_t v = (255.0f * (value + 1.0f)) / 2.0f;
  return std::clamp(v, 0, 255);
}

ScePadData getPadData(int handle) {
  LOG_USE_MODULE(libScePad);
  ScePadData data;

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;

  if (pController == nullptr) return ScePadData();
  auto lockSDL2 = accessVideoOut().getSDLLock();
  // SDL_GameControllerTouch

  data = ScePadData {
      .buttons = getButtons(pController),
      .leftStick =
          {
              .x = convertAnalog((float)SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f),
              .y = convertAnalog((float)SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f),
          },
      .rightStick =
          {
              .x = convertAnalog((float)SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f),
              .y = convertAnalog((float)SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f),
          },
      .analogButtons =
          {
              .l2 = convertAnalog((float)SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f),
              .r2 = convertAnalog((float)SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f),
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
    uint8_t s = 0;
    float   x = 0.0f, y = 0.0f;
    auto&   touch = data.touchData.touch[f];

    SDL_GameControllerGetTouchpadFinger(pController, 0, f, &s, &x, &y, NULL);
    if (s > 0) {
      touch.x = x * 0xFFFF, touch.y = y * 0xFFFF;
      ++data.touchData.touchNum;
    }
  }

  return data;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePad";

EXPORT SYSV_ABI int scePadInit(void) {
  LOG_USE_MODULE(libScePad);

  int32_t ret = SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) == 0 ? Ok : Err::FATAL;
  if (SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0)
    LOG_WARN(L"Failed to load game controller mappings");
  return ret;
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
    setupPad(n, userId);

    LOG_INFO(L"-> Pad[%llu]: userId:%d name:%S", n, userId, SDL_GameControllerNameForIndex(n));
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

  auto pData          = getData();
  auto pController    = &pData->controller[handle];
  auto pSDLController = pController->padPtr;
  if (SDL_GameControllerGetAttached(pSDLController) == false) {
    SDL_GameControllerClose(pSDLController);
    if ((pSDLController = setupPad(handle, 0)) == nullptr) return Err::DEVICE_NOT_CONNECTED;
  }

  std::unique_lock const lock(pData->m_mutexInt);

  *pPadData = getPadData(handle);

  int retVal = std::memcmp((uint8_t*)&pData->controller[handle].prePadData, (uint8_t*)pPadData, offsetof(ScePadData, connected));
  LOG_DEBUG(L"buttons 0x%lx diff:%d", pPadData->buttons, retVal);

  pData->controller[handle].prePadData = *pPadData;
  return abs(retVal);
}

EXPORT SYSV_ABI int scePadReadState(int32_t handle, ScePadData* pData) {
  auto ret = scePadRead(handle, pData, 1);
  return ret >= 0 ? Ok : ret;
}

EXPORT SYSV_ABI int scePadSetMotionSensorState(int32_t handle, bool bEnable) {
  if (handle < 0) return Err::INVALID_HANDLE;
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

  auto pData       = getData();
  auto pController = pData->controller[handle].padPtr;

  SDL_GameControllerRumble(pController, ((float)pParam->smallMotor / 255.0f) * 0xFFFF, ((float)pParam->largeMotor / 255.0f) * 0xFFFF, -1);
  return Ok;
}

EXPORT SYSV_ABI int scePadSetLightBar(int32_t handle, const ScePadColor* pParam) {
  if (handle < 0) return Err::INVALID_HANDLE;

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
  LOG_DEBUG(L"");
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
  LOG_DEBUG(L"");
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI int scePadDeviceClassGetExtendedInformation(int32_t handle, ScePadDeviceClassExtendedInformation* pExtInfo) {
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"");
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}
}
