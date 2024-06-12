#pragma once

#include "types.h"
#include "utility/utility.h"

#include <stdint.h>

enum class ControllerType {
  Unknown,
  Keyboard,
  Xinput,
  SDL,
};

enum class ControllerState {
  Unknown,
  Connected,
  Disconnected,
  Closed,
};

class IController {
  CLASS_NO_COPY(IController);
  CLASS_NO_MOVE(IController);

  protected:
  ControllerType const      m_type;
  ControllerState           m_state     = ControllerState::Disconnected;
  ScePadDeviceClass         m_class     = ScePadDeviceClass::STANDARD;
  PadPortType               m_port      = PadPortType::STANDARD;
  ScePadStickInformation    m_stickInfo = {2, 2}; // todo make config
  ScePadTouchPadInformation m_touchInfo = {44.86f, {1920, 950}};
  uint32_t                  m_userId;
  uint8_t                   m_connectCount;
  ScePadColor               m_lastColor = {0x00, 0x00, 0xFF};
  char                      m_guid[33];
  char                      m_name[33];

  IController(ControllerType type, uint32_t userid): m_type(type), m_userId(userid) {
    ::memset(m_guid, '0', sizeof(m_guid));
    ::strcpy_s(m_name, "[NOT YET CONNECTED]");
  }

  public:
  virtual ~IController() = default;

  auto getUserID() const { return m_userId; }

  auto getType() const { return m_type; }

  auto getState() const { return m_state; }

  auto getName() const { return m_name; }

  auto getGUID() const { return m_guid; }

  auto getConnectionsCount() const { return m_connectCount; }

  auto isConnected() { return m_state == ControllerState::Connected || reconnect(); }

  auto readControllerInfo(ScePadControllerInformation& pInfo) {
    pInfo = {
        .touchPadInfo   = m_touchInfo,
        .stickInfo      = m_stickInfo,
        .connectionType = (uint8_t)m_port,
        .connectedCount = m_connectCount,
        .connected      = isConnected(),
        .deviceClass    = m_class,
    };

    return true;
  }

  virtual bool reconnect()                                   = 0;
  virtual void close()                                       = 0;
  virtual bool readPadData(ScePadData& data)                 = 0;
  virtual bool setMotion(bool state)                         = 0;
  virtual bool resetOrientation()                            = 0;
  virtual bool setRumble(const ScePadVibrationParam* pParam) = 0;
  virtual bool setLED(const ScePadColor* pParam)             = 0;
  virtual bool resetLED()                                    = 0;

  static inline uint8_t scaleAnalogStick(int16_t value) {
    // input -32768 (up/left) to 32767 (down/right).
    // return 0..255

    return 255 * (32768 + value) / (32768 + 32767);
  }

  static inline uint8_t scaleAnalogButton(int16_t value) {
    // input 0 when released to 32767
    // return 0..255

    return 255 * value / 32767;
  }
};
