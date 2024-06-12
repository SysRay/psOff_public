#include "common.h"
#include "config_emu.h"
#include "core/timer/timer.h"
#include "interfaces/ikbd.h"
#include "interfaces/isdl.h"
#include "interfaces/ixip.h"
#include "logging.h"
#include "types.h"

#include <mutex>

LOG_DEFINE_MODULE(libScePad);

namespace {
constexpr uint32_t MAX_CONTROLLERS_COUNT     = 16;
constexpr int32_t  PSMOVE_DUMMY_HANDLE_RANGE = 1336;

struct Controller {
  int32_t userId = -1;

  std::unique_ptr<IController> backend;
  ScePadData                   prePadData;
};

struct Pimpl {
  Pimpl() = default;

  std::mutex m_mutexInt;

  std::array<Controller, MAX_CONTROLLERS_COUNT> controller;
};

static auto* getData() {
  static Pimpl obj;
  return &obj;
}

static ControllerType _getPadType(int32_t userId) {
  LOG_USE_MODULE(libScePad);

  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::CONTROLS);

  auto type = (*jData)["pads"][userId - 1]["type"];

  if (type == "keyboard") {
    return ControllerType::Keyboard;
  } else if (type == "sdl") {
    return ControllerType::SDL;
  } else if (type == "xinput") {
    return ControllerType::Xinput;
  }

  std::string temp;
  type.get_to(temp);
  LOG_ERR(L"Unknown controller backend \"%S\" falling back to \"SDL\"", temp.c_str());
  return ControllerType::SDL;
};

static int _padOpen(int32_t userId, PadPortType type, int32_t index, const void* pParam) {
  LOG_USE_MODULE(libScePad);
  if ((userId < 1 || userId > 4) && userId != 0xFF) return Err::Pad::INVALID_ARG;
  if (type == PadPortType::REMOTE_CONTROL && userId != 0xFF) return Err::Pad::INVALID_ARG;
  if (type != PadPortType::STANDARD && type == PadPortType::SPECIAL) {
    LOG_ERR(L"todo Handle PlayStation Move");
    return PSMOVE_DUMMY_HANDLE_RANGE + userId;
  }

  auto pData = getData();

  std::unique_lock const lock(pData->m_mutexInt);

  // Check already opened
  for (uint32_t n = 0; n < MAX_CONTROLLERS_COUNT; ++n) {
    if (pData->controller[n].userId == userId) return Err::Pad::ALREADY_OPENED;
  }
  // - already open

  for (int n = 0; n < MAX_CONTROLLERS_COUNT; ++n) {
    if (pData->controller[n].userId >= 0) continue;
    auto& padBackend = pData->controller[n].backend;

    pData->controller[n].prePadData = ScePadData();
    pData->controller[n].userId     = userId;

    switch (_getPadType(userId)) {
      case ControllerType::SDL: padBackend = createController_sdl(userId); break;

      case ControllerType::Xinput: padBackend = createController_xinput(userId); break;

      case ControllerType::Keyboard: padBackend = createController_keyboard(userId); break;

      default: LOG_CRIT(L"Unimplemented controller type!"); return Err::Pad::FATAL;
    }

    LOG_INFO(L"-> Pad[%d]: userId:%d name:%S guid:%S", n, userId, padBackend->getName(), padBackend->getGUID());
    return n + 1;
  }

  return Err::Pad::NO_HANDLE;
}

static inline Controller* getController(int32_t handle) {
  if (handle < 1 || handle >= MAX_CONTROLLERS_COUNT) return nullptr;
  return &getData()->controller[handle - 1];
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePad";

EXPORT SYSV_ABI int scePadInit(void) {
  LOG_USE_MODULE(libScePad);
  LOG_TRACE(L"scePadInit()");
  (void)getData();
  return Ok;
}

EXPORT SYSV_ABI int scePadOpen(int32_t userId, PadPortType type, int32_t index, const void* param) {
  return _padOpen(userId, type, index, param);
}

EXPORT SYSV_ABI int scePadOpenExt(int userId, PadPortType type, int index, const void* param) {
  return _padOpen(userId, type, index, param);
}

EXPORT SYSV_ABI int scePadClose(int32_t handle) {
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) return Ok; // Closing PlayStation Move
  std::unique_lock const lock(getData()->m_mutexInt);

  LOG_USE_MODULE(libScePad);
  LOG_INFO(L"<- Pad[%d]", handle);

  if (auto ctl = getController(handle)) {
    ctl->userId = -1;
    if (ctl->backend) ctl->backend->close();
    return Ok;
  }

  return Err::Pad::INVALID_ARG;
}

EXPORT SYSV_ABI int scePadGetHandle(int32_t userId, PadPortType type, int32_t index) {
  if (type == PadPortType::SPECIAL) return PSMOVE_DUMMY_HANDLE_RANGE + userId;
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"");

  auto                   pData = getData();
  std::unique_lock const lock(pData->m_mutexInt);

  for (uint32_t n = 0; n < MAX_CONTROLLERS_COUNT; ++n) {
    if (pData->controller[n].userId == userId) {
      return n + 1;
    }
  }

  return Err::Pad::NO_HANDLE;
}

EXPORT SYSV_ABI int scePadRead(int32_t handle, ScePadData* pPadData, int32_t num) {
  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move read");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;

    ctl->backend->readPadData(*pPadData);

    int retVal = std::memcmp((uint8_t*)&ctl->prePadData, (uint8_t*)pPadData, offsetof(ScePadData, connected));
    LOG_TRACE(L"buttons 0x%x leftStick:%u/%u rightStick:%u/%u L2/R2:%u,%u diff:%d", pPadData->buttons, pPadData->leftStick.x, pPadData->leftStick.y,
              pPadData->rightStick.x, pPadData->rightStick.y, pPadData->analogButtons.l2, pPadData->analogButtons.r2, retVal);

    ctl->prePadData = *pPadData;
    return abs(retVal);
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadReadState(int32_t handle, ScePadData* pData) {
  auto ret = scePadRead(handle, pData, 1);
  return ret >= 0 ? Ok : ret;
}

EXPORT SYSV_ABI int scePadSetMotionSensorState(int32_t handle, bool bEnable) {
  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move sensor");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;
    ctl->backend->setMotion(bEnable);
    return Ok;
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadSetTiltCorrectionState(int32_t handle, bool bEnable) {
  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move tiltcorrection");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);
  if (getController(handle) == nullptr) return Err::Pad::INVALID_HANDLE;

  return Ok;
}

EXPORT SYSV_ABI int scePadSetAngularVelocityDeadbandState(int32_t handle, bool bEnable) {
  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move velocity");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);
  if (getController(handle) == nullptr) return Err::Pad::INVALID_HANDLE;

  return Ok;
}

EXPORT SYSV_ABI int scePadResetOrientation(int32_t handle) {
  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move orientation reset");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;
    ctl->backend->resetOrientation();
    return Ok;
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadSetVibration(int32_t handle, const ScePadVibrationParam* pParam) {
  if (pParam == nullptr) return Err::Pad::INVALID_ARG;

  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move vibration");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;
    return ctl->backend->setRumble(pParam) ? Ok : Err::Pad::INVALID_ARG;
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadSetLightBar(int32_t handle, const ScePadColor* pParam) {
  if (pParam == nullptr) return Err::Pad::INVALID_ARG;

  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move lightbar");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;
    return ctl->backend->setLED(pParam) ? Ok : Err::Pad::INVALID_LIGHTBAR_SETTING;
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadResetLightBar(int32_t handle) {
  LOG_USE_MODULE(libScePad);
  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move lightbar reset");
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;
    return ctl->backend->resetLED() ? Ok : Err::Pad::INVALID_LIGHTBAR_SETTING;
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadGetControllerInformation(int32_t handle, ScePadControllerInformation* pInfo) {
  if (pInfo == nullptr) return Err::Pad::INVALID_ARG;
  LOG_USE_MODULE(libScePad);

  if (handle > PSMOVE_DUMMY_HANDLE_RANGE) {
    LOG_ERR(L"todo Handle PlayStation Move info");
    pInfo->deviceClass    = ScePadDeviceClass::STANDARD;
    pInfo->connectionType = (uint8_t)PadPortType::SPECIAL;
    pInfo->connected      = false;
    return Ok;
  }

  std::unique_lock const lock(getData()->m_mutexInt);

  if (auto ctl = getController(handle)) {
    if (ctl->backend == nullptr) return Err::Pad::FATAL;

    if (ctl->backend->readControllerInfo(*pInfo)) {
      LOG_DEBUG(L"handle:%d connected:%d", handle, pInfo->connected);
      return Ok;
    }

    return Err::Pad::FATAL;
  }

  return Err::Pad::INVALID_HANDLE;
}

EXPORT SYSV_ABI int scePadGetExtControllerInformation(int32_t handle, ScePadExtControllerInformation* pInfo) {
  std::unique_lock const lock(getData()->m_mutexInt);
  if (getController(handle) == nullptr) return Err::Pad::INVALID_HANDLE;
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int scePadDeviceClassParseData(int32_t handle, const ScePadData* pData, ScePadDeviceClassData* pDeviceClassData) {
  std::unique_lock const lock(getData()->m_mutexInt);
  if (getController(handle) == nullptr) return Err::Pad::INVALID_HANDLE;
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int scePadDeviceClassGetExtendedInformation(int32_t handle, ScePadDeviceClassExtendedInformation* pExtInfo) {
  std::unique_lock const lock(getData()->m_mutexInt);
  if (getController(handle) == nullptr) return Err::Pad::INVALID_HANDLE;
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void scePadTerminate() {
  auto pData = getData();

  std::unique_lock const lock(pData->m_mutexInt);

  for (int i = 0; i < MAX_CONTROLLERS_COUNT; i++) {
    if (auto backend = pData->controller[i].backend.get()) backend->close();
  }
}
}
