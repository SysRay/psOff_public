#include "cconfig.h"

#include "gamereport.h"
#include "logging.h"
#include "types.h"

#include <SDL.h>

LOG_DEFINE_MODULE(libScePad_config);

ControllerType MapControllerType(json& type) {
  LOG_USE_MODULE(libScePad_config);

  if (type == "keyboard" || type == "kb" || type == "kbd") {
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

static std::unordered_map<std::string_view, ControllerKey> jsonKeys = {
    {"l3", ControllerKey::L3},
    {"r3", ControllerKey::R3},
    {"options", ControllerKey::OPTIONS},
    {"up", ControllerKey::DPAD_UP},
    {"right", ControllerKey::DPAD_RIGHT},
    {"down", ControllerKey::DPAD_DOWN},
    {"left", ControllerKey::DPAD_LEFT},
    {"l2", ControllerKey::L2},
    {"r2", ControllerKey::R2},
    {"l1", ControllerKey::L1},
    {"r1", ControllerKey::R1},
    {"triangle", ControllerKey::TRIANGLE},
    {"circle", ControllerKey::CIRCLE},
    {"cross", ControllerKey::CROSS},
    {"square", ControllerKey::SQUARE},
    {"touchpad", ControllerKey::TOUCH_PAD},
    {"dpad_up", ControllerKey::DPAD_UP},
    {"dpad_down", ControllerKey::DPAD_DOWN},
    {"dpad_left", ControllerKey::DPAD_LEFT},
    {"dpad_right", ControllerKey::DPAD_RIGHT},
    {"lx+", ControllerKey::LX_UP},
    {"lx-", ControllerKey::LX_DOWN},
    {"ly+", ControllerKey::LY_UP},
    {"ly-", ControllerKey::LY_DOWN},
    {"rx+", ControllerKey::RX_UP},
    {"rx-", ControllerKey::RX_DOWN},
    {"ry+", ControllerKey::RY_UP},
    {"ry-", ControllerKey::RY_DOWN},
};

ControllerKey MapControllerKey(const std::string& key) {
  LOG_USE_MODULE(libScePad_config);

  if (jsonKeys.find(key) == jsonKeys.end()) {
    LOG_CRIT(L"Unknown keytype in controller config: %S", key.c_str());
    return ControllerKey::UNKNOWN_KEY;
  }

  return jsonKeys[key];
}

static SDL_Scancode ParseScanCodeName(const char* key, const char* name) {
  LOG_USE_MODULE(libScePad_config);

  auto code = SDL_GetScancodeFromName(name);
  if (code == GAMEREPORT_USER_SEND_SCANCODE) {
    LOG_ERR(L"Attempt to bind occupied key is prevented, please change your bind for gpkey:%S", key);
    return SDL_SCANCODE_UNKNOWN;
  }

  return code;
}

ControllerConfig::ControllerConfig() {
  LOG_USE_MODULE(libScePad_config);

  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::CONTROLS);

  auto& pads = (*jData)["pads"];

  std::string temps;

  for (auto& [key, val]: (*jData)["keybinds"].items()) {
    auto& mapEntry = m_keymap[(uint32_t)MapControllerKey(key)];
    val.get_to(temps);
    if (temps.size() == 0) {
      LOG_ERR(L"Unbound gpkey:%S", key.c_str());
      continue;
    }

    size_t fpos;
    if ((fpos = temps.find("%+")) != std::string::npos) { // Process keybinds with modifier
      std::string mod = temps.substr(0, fpos);
      mapEntry.key    = ParseScanCodeName(key.c_str(), temps.c_str() + fpos + 2);
      mapEntry.mod    = ParseScanCodeName(key.c_str(), mod.c_str());
      if (mapEntry.mod == 0 || mapEntry.key == 0) LOG_ERR(L"Failed to resolve bind for gpkey:%S, mapEntry(%d,%d)", key.c_str(), mapEntry.key, mapEntry.mod);
      continue;
    }

    mapEntry.mod = 0;
    if ((mapEntry.key = ParseScanCodeName(key.c_str(), temps.c_str())) == 0)
      LOG_ERR(L"Failed to resolve bind for gpkey:%S, unknown or occupied key: %S", key.c_str(), temps.c_str());
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
