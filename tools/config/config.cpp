#define __APICALL_EXTERN
#include "config.h"
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

  std::pair<boost::unique_lock<boost::mutex>, json*> accessModule(ConfigSaveFlags flag);

  virtual bool save(uint32_t flags);
};

IConfig* accessConfig() {
  static Config obj;
  return &obj;
}

std::pair<boost::unique_lock<boost::mutex>, json*> Config::accessModule(ConfigSaveFlags flag) {
  switch (flag) {
    case ConfigSaveFlags::LOGGING: m_future_logging.wait(); return std::make_pair(boost::unique_lock(m_mutex_logging), &m_logging);
    case ConfigSaveFlags::GRAPHICS: m_future_graphics.wait(); return std::make_pair(boost::unique_lock(m_mutex_graphics), &m_graphics);
    case ConfigSaveFlags::AUDIO: m_future_audio.wait(); return std::make_pair(boost::unique_lock(m_mutex_audio), &m_audio);
    case ConfigSaveFlags::CONTROLS: m_future_controls.wait(); return std::make_pair(boost::unique_lock(m_mutex_controls), &m_controls);

    default: throw std::exception("Invalid bit flag");
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
  if (flags & (uint32_t)ConfigSaveFlags::LOGGING) result &= save("logging.json", m_logging);
  if (flags & (uint32_t)ConfigSaveFlags::GRAPHICS) result &= save("graphics.json", m_graphics);
  if (flags & (uint32_t)ConfigSaveFlags::AUDIO) result &= save("audio.json", m_audio);
  if (flags & (uint32_t)ConfigSaveFlags::CONTROLS) result &= save("controls.json", m_controls);

  return true;
}

Config::Config() {
  auto load = [this](std::string_view fname, json* j, json defaults = {}, ConfigSaveFlags dflag = ConfigSaveFlags::NONE) {
    auto path          = std::string("./config/") + fname.data();
    bool should_resave = false;

    try {
      std::ifstream json_file(path);
      *j = json::parse(json_file, nullptr, true, true);
      printf("Config %s loaded successfully\n", fname.data());
    } catch (const json::exception& e) {
      if ((std::filesystem::exists(path))) printf("Failed to parse %s: %s\n", fname.data(), e.what());

      try {
        std::filesystem::path newp(path);
        newp.replace_extension(".back");
        std::filesystem::rename(path, newp);
      } catch (const std::filesystem::filesystem_error& e) {
      }

      *j = defaults;

      should_resave = true;
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
        should_resave = true;
        printf("%s: unused parameter \"%s\" has been removed!\n", fname.data(), ckey.c_str());
      }
    }

    if (should_resave && dflag != ConfigSaveFlags::NONE) this->save((uint32_t)dflag);
  };

  m_future_logging  = std::async(std::launch::async, load, std::string_view("logging.json"), &m_logging, json({{"sink", "baical"}, {"verbosity", 1}}),
                                 ConfigSaveFlags::LOGGING);
  m_future_graphics = std::async(std::launch::async, load, std::string_view("graphics.json"), &m_graphics, json({}), ConfigSaveFlags::GRAPHICS);
  m_future_audio =
      std::async(std::launch::async, load, std::string_view("audio.json"), &m_audio, json({{"volume", 0.5f}, {"device", "[default]"}}), ConfigSaveFlags::AUDIO);
  m_future_controls = std::async(
      std::launch::async, load, std::string_view("controls.json"), &m_controls,
      json({{"type", "gamepad"},
            {"deadzones", json::array({{{"left_stick", {{"x", 0.0f}, {"y", 0.0f}}}, {"right_stick", {{"x", 0.0f}, {"y", 0.0f}}}},
                                       {{"left_stick", {{"x", 0.0f}, {"y", 0.0f}}}, {"right_stick", {{"x", 0.0f}, {"y", 0.0f}}}},
                                       {{"left_stick", {{"x", 0.0f}, {"y", 0.0f}}}, {"right_stick", {{"x", 0.0f}, {"y", 0.0f}}}},
                                       {{"left_stick", {{"x", 0.0f}, {"y", 0.0f}}}, {"right_stick", {{"x", 0.0f}, {"y", 0.0f}}}}})},
            {"keybinds",
             {
                 {"triangle", ""}, {"square", ""},   {"circle", ""}, {"cross", ""}, {"dpad_up", ""}, {"dpad_down", ""}, {"dpad_left", ""}, {"dpad_right", ""},
                 {"options", ""},  {"touchpad", ""}, {"l1", ""},     {"l2", ""},    {"l3", ""},      {"r1", ""},        {"r2", ""},        {"r3", ""},
                 {"lx-", ""},      {"lx+", ""},      {"ly-", ""},    {"ly+", ""},   {"rx-", ""},     {"rx+", ""},       {"ry-", ""},       {"ry+", ""},
             }}}),
      ConfigSaveFlags::CONTROLS);
}
