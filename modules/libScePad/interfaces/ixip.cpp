#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "ikbd.h"
#include "logging.h"

#include <bitset>
#include <math.h>

LOG_DEFINE_MODULE(libScePad_xip);

static bool is_XIP_inited = false;

class XIPController: public IController {
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
  bool setRumble(const ScePadVibrationParam* pParam) final;
  bool setLED(const ScePadColor* pParam) final;
  bool resetLED() final;

  uint32_t getButtons();

  static void init();
};

std::unique_ptr<IController> createController_xinput(ControllerConfig* cfg, uint32_t userid) {
  return std::make_unique<XIPController>(cfg, userid);
}

void XIPController::init() {
  LOG_USE_MODULE(libScePad_xip);

  if (is_XIP_inited == false) {
    // todo: load xinput
    LOG_CRIT(L"XInput support is not ready yet");
    is_XIP_inited = true;
  }
}

void XIPController::close() {
  if (m_state == ControllerState::Disconnected || m_state == ControllerState::Closed) return;
  m_state = ControllerState::Closed;
}

bool XIPController::reconnect() {
  return false;
}

uint32_t XIPController::getButtons() {
  return 0; // todo
}

bool XIPController::readPadData(ScePadData& data) {
  auto lockSDL2 = accessVideoOut().getSDLLock();

  if (m_state == ControllerState::Closed) return false;

  data = ScePadData {
      .buttons = getButtons(),
      .leftStick =
          {
              .x = 0,
              .y = 0,
          },
      .rightStick =
          {
              .x = 0,
              .y = 0,
          },
      .analogButtons =
          {
              .l2 = 0,
              .r2 = 0,
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
  return true;
}

bool XIPController::setRumble(const ScePadVibrationParam* pParam) {
  return true;
}

bool XIPController::setLED(const ScePadColor* pParam) {
  m_lastColor = *pParam;
  return true;
}

bool XIPController::resetLED() {
  m_lastColor = {0x00, 0x00, 0xff};
  return setLED(&m_lastColor);
}
