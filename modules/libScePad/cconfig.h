#pragma once

#include "config_emu.h"
#include "types.h"

class ControllerConfig {
  struct Key {
    uint16_t key;
    uint16_t mod;
  } m_keymap[(uint32_t)ControllerKey::CONTROLLER_KEYS_COUNT];

  struct DeadZone {
    float x, y;
  };

  struct Settings {
    ControllerType type;
    DeadZone       ls, rs;
  } m_pads[4];

  public:
  auto GetPadType(int n) const {
    if (n < 1 || n > 4) return ControllerType::Unknown;
    return m_pads[n - 1].type;
  }

  auto GetBind(ControllerKey key) const { return m_keymap[(uint32_t)key]; }

  ControllerConfig();
  virtual ~ControllerConfig() = default;
};
