#define __APICALL_EXTERN
#include "config_emu.h"
#undef __APICALL_EXTERN

#include <fstream>
#include <future>

class Config: public IConfig {
  json m_logging;
  json m_graphics;
  json m_audio;
  json m_controls;

  boost::mutex m_mutex_logging;
  boost::mutex m_mutex_graphics;
  boost::mutex m_mutex_audio;
  boost::mutex m_mutex_controls;

  std::future<void> m_future_logging;
  std::future<void> m_future_graphics;
  std::future<void> m_future_audio;
  std::future<void> m_future_controls;

  public:
  Config();
  virtual ~Config() = default;

  std::pair<boost::unique_lock<boost::mutex>, json*> accessModule(ConfigModFlag flag);

  virtual bool save(uint32_t flags);
};

IConfig* accessConfig() {
  static Config obj;
  return &obj;
}

std::pair<boost::unique_lock<boost::mutex>, json*> Config::accessModule(ConfigModFlag flag) {
  switch (flag) {
    case ConfigModFlag::LOGGING: m_future_logging.wait(); return std::make_pair(boost::unique_lock(m_mutex_logging), &m_logging);
    case ConfigModFlag::GRAPHICS: m_future_graphics.wait(); return std::make_pair(boost::unique_lock(m_mutex_graphics), &m_graphics);
    case ConfigModFlag::AUDIO: m_future_audio.wait(); return std::make_pair(boost::unique_lock(m_mutex_audio), &m_audio);
    case ConfigModFlag::CONTROLS: m_future_controls.wait(); return std::make_pair(boost::unique_lock(m_mutex_controls), &m_controls);

    default: printf("Invalid bit flag!\n"); exit(1);
  }
}

bool Config::save(uint32_t flags) {
  auto save = [this](std::string_view fname, json& j) {
    auto path = std::string("./config/") + fname.data();
    try {
      std::ofstream json_file(path);
      json_file << std::setw(2) << j;
      return true;
    } catch (const json::exception& e) {
      printf("Failed to save %s: %s\n", fname.data(), e.what());
      return false;
    }
  };

  bool result = true;

  if (!std::filesystem::is_directory("./config/")) std::filesystem::create_directory("./config/");
  if (flags & (uint32_t)ConfigModFlag::LOGGING) result &= save("logging.json", m_logging);
  if (flags & (uint32_t)ConfigModFlag::GRAPHICS) result &= save("graphics.json", m_graphics);
  if (flags & (uint32_t)ConfigModFlag::AUDIO) result &= save("audio.json", m_audio);
  if (flags & (uint32_t)ConfigModFlag::CONTROLS) result &= save("controls.json", m_controls);

  return true;
}

Config::Config() {
  auto load = [this](std::string_view fname, json* j, json defaults = {}, ConfigModFlag dflag = ConfigModFlag::NONE) {
    auto path          = std::string("./config/") + fname.data();
    bool should_resave = false, should_backup = false;

    try {
      std::ifstream json_file(path);
      *j = json::parse(json_file, nullptr, true, true);

      if ((*j).type() == defaults.type()) {
        printf("Config %s loaded successfully\n", fname.data());
      } else {
        should_backup = true;
        should_resave = true;
      }
    } catch (const json::exception& e) {
      if ((std::filesystem::exists(path))) printf("Failed to parse %s: %s\n", fname.data(), e.what());

      *j            = defaults;
      should_resave = true;
      should_backup = true;
    }

    for (auto& [dkey, dval]: defaults.items()) {
      if ((*j)[dkey].is_null() && !dval.is_null()) {
        (*j)[dkey]    = dval;
        should_resave = true;
        printf("%s: missing parameter \"%s\" has been added!\n", fname.data(), dkey.c_str());
      }
    }

    for (auto& [ckey, cval]: j->items()) {
      if (defaults[ckey].is_null()) {
        j->erase(ckey);
        should_backup = true;
        should_resave = true;
        printf("%s: unused parameter \"%s\" has been removed!\n", fname.data(), ckey.c_str());
      }
    }

    if (should_backup == true) {
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

  const json defaultpad = {{"type", "gamepad"}, {"deadzones", {{"left_stick", {{"x", 0.0f}, {"y", 0.0f}}}, {"right_stick", {{"x", 0.0f}, {"y", 0.0f}}}}}};

  m_future_logging =
      std::async(std::launch::async, load, std::string_view("logging.json"), &m_logging, json({{"sink", "FileTxt"}, {"verbosity", 1}}), ConfigModFlag::LOGGING);
  m_future_graphics = std::async(std::launch::async, load, std::string_view("graphics.json"), &m_graphics, json::object({}), ConfigModFlag::GRAPHICS);
  m_future_audio =
      std::async(std::launch::async, load, std::string_view("audio.json"), &m_audio, json({{"volume", 0.5f}, {"device", "[default]"}}), ConfigModFlag::AUDIO);
  m_future_controls = std::async(std::launch::async, load, std::string_view("controls.json"), &m_controls,
                                 json({{"pads", json::array({defaultpad, defaultpad, defaultpad, defaultpad})},
                                       {"keybinds",
                                        {
                                            {"triangle", "i"}, {"square", "j"},       {"circle", "l"},       {"cross", "k"},
                                            {"dpad_up", "up"}, {"dpad_down", "down"}, {"dpad_left", "left"}, {"dpad_right", "right"},
                                            {"options", "f1"}, {"touchpad", "f4"},    {"l1", "f3"},          {"l2", "f5"},
                                            {"l3", "space"},   {"r1", "f2"},          {"r2", "f6"},          {"r3", "home"},
                                            {"lx-", "a"},      {"lx+", "d"},          {"ly-", "w"},          {"ly+", "s"},
                                            {"rx-", "h"},      {"rx+", "f"},          {"ry-", "t"},          {"ry+", "g"},
                                        }}}),
                                 ConfigModFlag::CONTROLS);
}
