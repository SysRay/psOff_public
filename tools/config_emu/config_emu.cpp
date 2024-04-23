#define __APICALL_EXTERN
#include "config_emu.h"
#undef __APICALL_EXTERN

#include <fstream>
#include <future>

namespace {
class Item {
  public:
  std::string_view const _name;

  bool              _dontfix; // If this flag is set then load function won't try fix the file according to default object
  json              _json;
  boost::mutex      _mutex;
  std::future<void> _future;

  Item(std::string_view name, bool df = false): _name(name), _dontfix(df) {}

  std::pair<boost::unique_lock<boost::mutex>, json*> access() {
    _future.wait();
    return std::make_pair(boost::unique_lock(_mutex), &_json);
  }

  bool save() {
    auto path = std::string("./config/") + _name.data();
    try {
      std::ofstream json_file(path);
      json_file << std::setw(2) << _json;
      return true;
    } catch (const json::exception& e) {
      printf("Failed to save %s: %s\n", _name.data(), e.what());
      return false;
    }
  };
};

static inline bool isJsonTypesSimilar(json& a, json& b) {
  return (a.type() == b.type()) || (a.is_number_integer() == b.is_number_integer());
}
} // namespace

class Config: public IConfig {

  Item m_logging  = {"logging.json"};
  Item m_graphics = {"graphics.json"};
  Item m_audio    = {"audio.json"};
  Item m_controls = {"controls.json"};
  Item m_general  = {"general.json"};
  Item m_resolve  = {"resolve.json", true /*dontfix flag*/};

  public:
  Config();
  virtual ~Config() = default;

  std::pair<boost::unique_lock<boost::mutex>, json*> accessModule(ConfigModFlag flag);

  virtual bool save(uint32_t flags) final;
};

IConfig* accessConfig() {
  static Config obj;
  return &obj;
}

std::pair<boost::unique_lock<boost::mutex>, json*> Config::accessModule(ConfigModFlag flag) {
  switch (flag) {
    case ConfigModFlag::LOGGING: return m_logging.access();
    case ConfigModFlag::GRAPHICS: return m_graphics.access();
    case ConfigModFlag::AUDIO: return m_audio.access();
    case ConfigModFlag::CONTROLS: return m_controls.access();
    case ConfigModFlag::GENERAL: return m_general.access();
    case ConfigModFlag::RESOLVE: return m_resolve.access();

    default: printf("Invalid bit flag!\n"); exit(1);
  }
}

bool Config::save(uint32_t flags) {

  bool result = true;

  if (!std::filesystem::is_directory("./config/")) std::filesystem::create_directory("./config/");
  if (flags & (uint32_t)ConfigModFlag::LOGGING) result &= m_logging.save();
  if (flags & (uint32_t)ConfigModFlag::GRAPHICS) result &= m_graphics.save();
  if (flags & (uint32_t)ConfigModFlag::AUDIO) result &= m_audio.save();
  if (flags & (uint32_t)ConfigModFlag::CONTROLS) result &= m_controls.save();
  if (flags & (uint32_t)ConfigModFlag::GENERAL) result &= m_general.save();
  if (flags & (uint32_t)ConfigModFlag::RESOLVE) result &= m_resolve.save();

  return true;
}

Config::Config() {
  auto load = [this](Item* item, json defaults = {}, ConfigModFlag dflag = ConfigModFlag::NONE) {
    auto path          = std::string("./config/") + item->_name.data();
    bool should_resave = false, should_backup = false;

    try {
      std::ifstream json_file(path);
      item->_json = json::parse(json_file, nullptr, true, true);

      if ((item->_json).type() == defaults.type()) {
        printf("Config %s loaded successfully\n", item->_name.data());
      } else {
        should_backup = true;
        should_resave = true;
      }
    } catch (const json::exception& e) {
      if ((std::filesystem::exists(path))) printf("Failed to parse %s: %s\n", item->_name.data(), e.what());

      item->_json   = defaults;
      should_resave = true;
      should_backup = true;
    }

    std::function<bool(json&, json&)> fixMissing;
    std::function<bool(json&, json&)> removeUnused;

    /**
     * @brief This function is necessary since json.items()
     * translates every key to string. Even the array indexes,
     * so we have to translate it back to number for arrays.
     *
     */
    auto getVal = [](json& _o, std::string_view key) -> json& {
      if (_o.is_array()) {
        int  _tempi;
        auto res = std::from_chars(key.data(), key.data() + key.size(), _tempi);
        if (res.ec == std::errc::invalid_argument) {
          printf("Unreachable scenario!\n"); // At least it should be
          exit(1);
        }
        return _o[_tempi];
      }

      return _o[key];
    };

    fixMissing = [&should_backup, &getVal, &fixMissing](json& obj, json& def) -> bool {
      bool missing = false;

      for (auto& [dkey, dval]: def.items()) {
        json& cval = getVal(obj, dkey);

        if ((cval.is_null() && !dval.is_null()) || !isJsonTypesSimilar(cval, dval)) {
          cval    = dval;
          missing = true;
        } else if (dval.is_structured()) {
          if (cval.is_structured()) { // Function calls itself if json element is array or object
            missing |= fixMissing(cval, dval);
            continue;
          }

          should_backup = true;
          cval          = dval;
          missing       = true;
        }
      }

      return missing;
    };

    if (!item->_dontfix && fixMissing(item->_json, defaults)) {
      should_resave = true;
      printf("%s: some missing or invalid parameters has been fixed!\n", item->_name.data());
    }

    // Just the same thing as above, but for removing unused keys this time
    removeUnused = [&getVal, &removeUnused](json& obj, json& def) -> bool {
      bool unused = false;

      for (auto& [ckey, cval]: obj.items()) {
        json& dval = getVal(def, ckey);

        if (dval.is_null()) {
          obj.erase(ckey);
          unused = true;
        } else if (dval.is_structured()) {
          unused |= removeUnused(cval, dval);
          continue;
        }
      }

      return unused;
    };

    if (!item->_dontfix && removeUnused(item->_json, defaults)) {
      should_backup = true;
      should_resave = true;
      printf("%s: some unused parameters has been removed!\n", item->_name.data());
    }

    if (should_backup == true && std::filesystem::exists(path)) {
      try {
        std::filesystem::path newp(path);
        newp.replace_extension(".back");
        std::filesystem::rename(path, newp);
      } catch (const std::filesystem::filesystem_error& e) {
        printf("%s: failed to create a backup: %s", path.c_str(), e.what());
      }
    }

    if (should_resave && dflag != ConfigModFlag::NONE) this->save((uint32_t)dflag);
  };

  json defprofiles = json::array();
  json defpads     = json::array();

  static const char* colors[4] = {"blue", "red", "green", "pink"};

  for (int i = 0; i < 4; i++) {
    defprofiles[i] = {{"name", std::format("Anon #{}", i + 1)}, {"color", colors[i]}};
    defpads[i]     = {
        {"type", "gamepad"},
        {"deadzones",
             {
             {"left_stick", {{"x", 0.0f}, {"y", 0.0f}}},
             {"right_stick", {{"x", 0.0f}, {"y", 0.0f}}},
         }},
    };
  }

  m_logging._future = std::async(std::launch::async | std::launch::deferred, load, &m_logging,
                                 json({{"$schema", "./.schemas/logging.json"}, {"sink", "FileBin"}, {"verbosity", 1}}), ConfigModFlag::LOGGING);

  m_graphics._future = std::async(
      std::launch::async | std::launch::deferred, load, &m_graphics,
      json({{"$schema", "./.schemas/graphics.json"}, {"display", 0u}, {"fullscreen", false}, {"width", 1920u}, {"height", 1080u}, {"xpos", -1}, {"ypos", -1}}),
      ConfigModFlag::GRAPHICS);

  m_audio._future = std::async(std::launch::async | std::launch::deferred, load, &m_audio,
                               json({{"$schema", "./.schemas/audio.json"}, {"device", "[default]"}, {"volume", 0.5f}}), ConfigModFlag::AUDIO);

  m_controls._future = std::async(std::launch::async | std::launch::deferred, load, &m_controls,
                                  json({
                                      {"$schema", "./.schemas/controls.json"},
                                      {"pads", defpads},
                                      {"keybinds",
                                       {
                                           {"triangle", "i"}, {"square", "j"},       {"circle", "l"},       {"cross", "k"},
                                           {"dpad_up", "up"}, {"dpad_down", "down"}, {"dpad_left", "left"}, {"dpad_right", "right"},
                                           {"options", "f1"}, {"touchpad", "f4"},    {"l1", "f3"},          {"l2", "f5"},
                                           {"l3", "space"},   {"r1", "f2"},          {"r2", "f6"},          {"r3", "home"},
                                           {"lx-", "a"},      {"lx+", "d"},          {"ly-", "w"},          {"ly+", "s"},
                                           {"rx-", "h"},      {"rx+", "f"},          {"ry-", "t"},          {"ry+", "g"},
                                       }},
                                  }),
                                  ConfigModFlag::CONTROLS);

  m_general._future = std::async(std::launch::async, load, &m_general,
                                 json({
                                     {"$schema", "./.schemas/general.json"},
                                     {"systemlang", 1u},
                                     {"netEnabled", false},
                                     {"netMAC", "00:00:00:00:00:00"},
                                     {"userIndex", 1u},
                                     {"onlineUsers", 1u},
                                     {"profiles", defprofiles},
                                 }),
                                 ConfigModFlag::GENERAL);

  m_resolve._future = std::async(std::launch::async | std::launch::deferred, load, &m_resolve, json({{"localhost", "127.0.0.1"}}), ConfigModFlag::RESOLVE);
}
