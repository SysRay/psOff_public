#include "common.h"
#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "interfaces/isdl.h"
#include "config_emu.h"
#include "logging.h"
#include "types.h"

#include <algorithm>
#include <chrono>

LOG_DEFINE_MODULE(libScePad);

namespace {
struct Controller {
  int32_t userId       = -1;

  std::unique_ptr<IController> padPtr;
  ScePadData                   prePadData;
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

// static SDL_GameController* setupPad(int n, int userId) {
//   auto pData       = getData();
//   auto pController = SDL_GameControllerOpen(n);
//   if (pController == nullptr) return nullptr;

//   SDL_GameControllerSetPlayerIndex(pController, userId - 1);
//   if (userId > 0) pData->controller[n].userId = userId;
//   pData->controller[n].prePadData = ScePadData();
//   pData->controller[n].padPtr     = pController;
//   ++pData->controller[n].countConnect;

//   return pController;
// }
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePad";

int initGamepadConfig(json* jData) {
  LOG_USE_MODULE(libScePad);

  // return ret ? Ok : Err::FATAL;
  return Ok;
}

int initKeyboardConfig(json* jData) {
  // SDL_GetKeyFromName();
  return Err::FATAL;
}

int initXinputConfig(json* jData) {
  /* todo: option to switch with config to pure xinput implementation without SDL2 */
  return Err::FATAL;
}

int readPadConfig() {
  LOG_USE_MODULE(libScePad);

  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::CONTROLS);
  auto& ctype = (*jData)["type"];

  if (ctype == "gamepad") {
    return initGamepadConfig(jData);
  } else if (ctype == "xinput") {
    return initXinputConfig(jData);
  } else if (ctype == "keyboard") {
    return initKeyboardConfig(jData);
  }

  try {
    std::string name;
    ctype.get_to(name);
    LOG_CRIT(L"Unknown controller type: %S", name.c_str());
  } catch (const json::exception& e) {
    LOG_CRIT(L"Failed to read controller configuration: %S", e.what());
  }

  return Err::FATAL;
}

EXPORT SYSV_ABI int scePadInit(void) {
  LOG_USE_MODULE(libScePad);
  return readPadConfig();
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
    auto& pController = pData->controller[n].padPtr;

    if (pController == nullptr) {
      pController = createController_sdl(userId);
    }

    LOG_INFO(L"-> Pad[%llu]: userId:%d name:%S guid:%S", n, userId, pController->getName(), pController->getGUID());
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

  pData->controller[handle].padPtr->close();
  // SDL_GameControllerClose();
  // pData->controller[handle].padPtr = nullptr;

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
  auto& pController = pData->controller[handle].padPtr;
  // if (SDL_GameControllerGetAttached(pController) == false) {
  //   SDL_GameControllerClose(pController);
  //   if ((pController = setupPad(handle, 0)) == nullptr) return Err::DEVICE_NOT_CONNECTED;
  // }

  std::unique_lock const lock(pData->m_mutexInt);

  pController->readPadData(*pPadData);

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

  auto pData        = getData();
  auto& pController = pData->controller[handle].padPtr;
  return pController->setMotion(bEnable);
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

  auto pData        = getData();
  auto& pController = pData->controller[handle].padPtr;

  pController->setRumble(pParam);
  return Ok;
}

EXPORT SYSV_ABI int scePadSetLightBar(int32_t handle, const ScePadColor* pParam) {
  if (handle < 0) return Err::INVALID_HANDLE;
  if (pParam == nullptr) return Err::INVALID_ARG;

  auto pData        = getData();
  auto& pController = pData->controller[handle].padPtr;

  if (!pController->setLED(pParam)) return Err::INVALID_LIGHTBAR_SETTING;

  return Ok;
}

EXPORT SYSV_ABI int scePadResetLightBar(int32_t handle) {
  if (handle < 0) return Err::INVALID_HANDLE;

  auto pData        = getData();
  auto& pController = pData->controller[handle].padPtr;
  if (pController->resetLED() == false) return Err::INVALID_LIGHTBAR_SETTING;

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
    pInfo->connectedCount          = pad.padPtr->getConnectionsCount();
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
  pInfo->connectedCount = pad.padPtr->getConnectionsCount();
  pInfo->connected      = pad.padPtr->isConnected();
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
  
}
}
