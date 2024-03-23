#include "cconfig.h"
#include "logging.h"

#include <SDL.h>

LOG_DEFINE_MODULE(libScePad_config);

ControllerType MapControllerType(json& type) {
  LOG_USE_MODULE(libScePad_config);

  if (type == "keyboard" || type == "kb") {
    return ControllerType::Keyboard;
  } else if (type == "sdl" || type == "gamepad") {
    return ControllerType::SDL;
  } else if (type == "xinput" || type == "xbox") {
    return ControllerType::Xinput;
  }

  std::string temp;
  type.get_to(temp);
  LOG_CRIT(L"Unknown controller type: %S", temp.c_str());
  return ControllerType::Unknown;
}

uint32_t MapControllerKey(const std::string& key) {
  LOG_USE_MODULE(libScePad_config);

  if (key == "l3") {
    return ControllerKey::L3;
  } else if (key == "r3") {
    return ControllerKey::R3;
  } else if (key == "options") {
    return ControllerKey::OPTIONS;
  } else if (key == "up") {
    return ControllerKey::DPAD_UP;
  } else if (key == "right") {
    return ControllerKey::DPAD_RIGHT;
  } else if (key == "down") {
    return ControllerKey::DPAD_DOWN;
  } else if (key == "left") {
    return ControllerKey::DPAD_LEFT;
  } else if (key == "l2") {
    return ControllerKey::L2;
  } else if (key == "r2") {
    return ControllerKey::R2;
  } else if (key == "l1") {
    return ControllerKey::L1;
  } else if (key == "r1") {
    return ControllerKey::R1;
  } else if (key == "triangle") {
    return ControllerKey::TRIANGLE;
  } else if (key == "circle") {
    return ControllerKey::CIRCLE;
  } else if (key == "cross") {
    return ControllerKey::CROSS;
  } else if (key == "square") {
    return ControllerKey::SQUARE;
  } else if (key == "touchpad") {
    return ControllerKey::TOUCH_PAD;
  } else if (key == "dpad_up") {
    return ControllerKey::DPAD_UP;
  } else if (key == "dpad_down") {
    return ControllerKey::DPAD_DOWN;
  } else if (key == "dpad_left") {
    return ControllerKey::DPAD_LEFT;
  } else if (key == "dpad_right") {
    return ControllerKey::DPAD_RIGHT;
  } else if (key == "lx+") {
    return ControllerKey::LX_UP;
  } else if (key == "lx-") {
    return ControllerKey::LX_DOWN;
  } else if (key == "ly+") {
    return ControllerKey::LY_UP;
  } else if (key == "ly-") {
    return ControllerKey::LY_DOWN;
  } else if (key == "rx+") {
    return ControllerKey::RX_UP;
  } else if (key == "rx-") {
    return ControllerKey::RX_DOWN;
  } else if (key == "ry+") {
    return ControllerKey::RY_UP;
  } else if (key == "ry-") {
    return ControllerKey::RY_DOWN;
  }

  LOG_CRIT(L"Unknown keytype in controller config: %S", key.c_str());
  return -1;
}

ControllerConfig::ControllerConfig() {
  LOG_USE_MODULE(libScePad_config);

  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::CONTROLS);

  auto& pads = (*jData)["pads"];

  std::string temps;

  for (auto& [key, val] : (*jData)["keybinds"].items()) {
    auto& mapEntry = m_keymap[(uint32_t)MapControllerKey(key)];
    val.get_to(temps);
    if (temps.size() == 0) {
      LOG_ERR(L"Unbound gpkey:%S", key.c_str());
      continue;
    }

    size_t fpos;
    if ((fpos = temps.find("%+")) != std::string::npos) { // Process keybinds with modifier
      std::string mod = temps.substr(0, fpos);
      mapEntry.key = SDL_GetScancodeFromName(temps.c_str() + fpos + 2);
      mapEntry.mod = SDL_GetScancodeFromName(mod.c_str());
      if (mapEntry.mod == 0 || mapEntry.key == 0)
        LOG_ERR(L"Failed to resolve bind for gpkey:%S, mapEntry(%d,%d)", key.c_str(), mapEntry.key, mapEntry.mod);
      continue;
    }

    mapEntry.mod = 0;
    if ((mapEntry.key = SDL_GetScancodeFromName(temps.c_str())) == 0)
      LOG_ERR(L"Failed to resolve bind for gpkey:%S, unknown key: %S", key.c_str(), temps.c_str());
  }

  for (int i = 0; i < sizeof(m_pads) / sizeof(*m_pads); i++) {
    auto& pad = pads[i];

    try {
      m_pads[i].type = MapControllerType(pad["type"]);
      auto& dls = pad["deadzones"]["left_stick"];
      auto& drs = pad["deadzones"]["right_stick"];

      dls["x"].get_to(m_pads[i].ls.x);
      dls["y"].get_to(m_pads[i].ls.y);
      drs["x"].get_to(m_pads[i].rs.x);
      drs["y"].get_to(m_pads[i].rs.y);
    } catch (const json::exception& e) {
      LOG_CRIT(L"Failed to read controllers config file: %S", e.what());
    }
  }
}
