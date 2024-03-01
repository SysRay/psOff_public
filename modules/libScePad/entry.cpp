#include "common.h"
#include "logging.h"
#include "types.h"

#include <timer.h>
#include <videoOut.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <bitset>
#include <chrono>

LOG_DEFINE_MODULE(libScePad);

namespace {
struct Controller {
  int32_t userId       = -1;
  uint8_t countConnect = 0;

  ScePadData prePadData;
};

struct Pimpl {
  Pimpl() = default;
  std::mutex m_mutexInt;

  std::array<Controller, GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1> controller;
};

static auto* getData() {
  static Pimpl obj;
  return &obj;
}

uint32_t getButtons(GLFWgamepadstate const& state) {
  std::bitset<32> bits;
  bits[(uint32_t)ScePadButtonDataOffset::L3]        = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
  bits[(uint32_t)ScePadButtonDataOffset::R3]        = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
  bits[(uint32_t)ScePadButtonDataOffset::OPTIONS]   = state.buttons[GLFW_GAMEPAD_BUTTON_START];
  bits[(uint32_t)ScePadButtonDataOffset::UP]        = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
  bits[(uint32_t)ScePadButtonDataOffset::RIGHT]     = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
  bits[(uint32_t)ScePadButtonDataOffset::DOWN]      = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
  bits[(uint32_t)ScePadButtonDataOffset::LEFT]      = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
  bits[(uint32_t)ScePadButtonDataOffset::L1]        = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
  bits[(uint32_t)ScePadButtonDataOffset::R1]        = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
  bits[(uint32_t)ScePadButtonDataOffset::TRIANGLE]  = state.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE];
  bits[(uint32_t)ScePadButtonDataOffset::CIRCLE]    = state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE];
  bits[(uint32_t)ScePadButtonDataOffset::CROSS]     = state.buttons[GLFW_GAMEPAD_BUTTON_CROSS];
  bits[(uint32_t)ScePadButtonDataOffset::SQUARE]    = state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE];
  bits[(uint32_t)ScePadButtonDataOffset::TOUCH_PAD] = state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
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

  GLFWgamepadstate state;
  if (auto res = glfwGetGamepadState(handle, &state); res < 0) {
    LOG_ERR(L"glfwGetGamepadState err:%d", res);
    return ScePadData();
  }

  auto pData    = getData();
  auto lockGlfw = accessVideoOut().getGlfwLock();
  return ScePadData {
      .buttons = getButtons(state),
      .leftStick =
          {
              .x = convertAnalog(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]),
              .y = convertAnalog(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]),
          },
      .rightStick =
          {
              .x = convertAnalog(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]),
              .y = convertAnalog(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]),
          },
      .analogButtons =
          {
              .l2 = convertAnalog(state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]),
              .r2 = convertAnalog(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]),
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

      .connected           = glfwJoystickIsGamepad(handle) == GLFW_TRUE,
      .timestamp           = accessTimer().getTicks(),
      .connectedCount      = pData->controller[handle].countConnect,
      .deviceUniqueDataLen = 0,
  };
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePad";

EXPORT SYSV_ABI int scePadInit(void) {
  glfwInit();
  return glfwInit() ? Ok : Err::FATAL;
}

EXPORT SYSV_ABI int scePadOpen(int32_t userId, PadPortType type, int32_t index, const void* pParam) {
  LOG_USE_MODULE(libScePad);
  auto pData = getData();

  std::unique_lock const lock(pData->m_mutexInt);

  // Check already opened
  for (size_t n = GLFW_JOYSTICK_1; n <= GLFW_JOYSTICK_LAST; ++n) {
    if (pData->controller[n].userId == userId) return Err::ALREADY_OPENED;
  }
  // - already open

  auto lockGlfw = accessVideoOut().getGlfwLock();
  for (size_t n = GLFW_JOYSTICK_1; n <= GLFW_JOYSTICK_LAST; ++n) {
    if (pData->controller[n].userId >= 0) continue;
    pData->controller[n].userId = userId;
    ++pData->controller[n].countConnect;
    pData->controller[n].prePadData = ScePadData();

    LOG_INFO(L"-> Pad[%llu]: userId:%d name:%S", n, userId, glfwGetJoystickName(n));
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

  return Ok;
}

EXPORT SYSV_ABI int scePadGetHandle(int32_t userId, PadPortType type, int32_t index) {
  auto pData = getData();
  LOG_USE_MODULE(libScePad);
  LOG_DEBUG(L"");
  std::unique_lock const lock(pData->m_mutexInt);

  for (size_t n = GLFW_JOYSTICK_1; n <= GLFW_JOYSTICK_LAST; ++n) {
    if (pData->controller[n].userId == userId) {
      return n;
    }
  }
  return Err::NO_HANDLE;
}

EXPORT SYSV_ABI int scePadRead(int32_t handle, ScePadData* pPadData, int32_t num) {
  LOG_USE_MODULE(libScePad);
  if (handle < 0) return Err::INVALID_HANDLE;

  auto pData = getData();

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
  return Ok;
}

EXPORT SYSV_ABI int scePadSetLightBar(int32_t handle, const ScePadColor* pParam) {
  if (handle < 0) return Err::INVALID_HANDLE;
  return Ok;
}

EXPORT SYSV_ABI int scePadResetLightBar(int32_t handle) {
  if (handle < 0) return Err::INVALID_HANDLE;
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

  auto lockGlfw         = accessVideoOut().getGlfwLock();
  pInfo->connectionType = (uint8_t)PadPortType::STANDARD;
  pInfo->connectedCount = pad.countConnect;
  pInfo->connected      = glfwJoystickPresent(handle) == GLFW_TRUE;
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