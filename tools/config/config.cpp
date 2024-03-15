#define __APICALL_EXTERN
#include "config.h"
#undef __APICALL_EXTERN

#include <fstream>

using json = nlohmann::json;

class Config: public IConfig {
  json         m_logging;
  json         m_graphics;
  json         m_audio;
  json         m_controls;
  boost::mutex m_mutex_logging;
  boost::mutex m_mutex_graphics;
  boost::mutex m_mutex_audio;
  boost::mutex m_mutex_controls;

  public:
  Config()          = default;
  virtual ~Config() = default;

  std::pair<boost::unique_lock<boost::mutex>, json*> accessModule(ConfigSaveFlags flag);

  virtual bool load();
  virtual bool save(uint32_t flags);
};

IConfig* accessConfig() {
  static Config obj;
  return &obj;
}

std::pair<boost::unique_lock<boost::mutex>, json*> Config::accessModule(ConfigSaveFlags flag) {
  switch (flag) {
    case ConfigSaveFlags::LOGGING: return std::make_pair(std::move(boost::unique_lock(m_mutex_logging)), &m_logging);
    case ConfigSaveFlags::GRAPHICS: return std::make_pair(std::move(boost::unique_lock(m_mutex_graphics)), &m_graphics);
    case ConfigSaveFlags::AUDIO: return std::make_pair(std::move(boost::unique_lock(m_mutex_audio)), &m_audio);
    case ConfigSaveFlags::CONTROLS: return std::make_pair(std::move(boost::unique_lock(m_mutex_controls)), &m_controls);

    default: throw std::exception("Invalid bit flag");
  }
}

bool Config::load() {
  auto load = [this](std::string_view fname, json& j, json defaults = {}, ConfigSaveFlags dflag = ConfigSaveFlags::NONE) {
    auto path = std::string("./config/") + fname.data();

    try {
      std::ifstream json_file(path);
      j = json::parse(json_file, nullptr, true, true);
    } catch (const json::exception& e) {
      printf("Failed to parse %s: %s\n", fname.data(), e.what());

      std::filesystem::path newp(path);
      newp.replace_extension(".back");
      std::filesystem::rename(path, newp);

      j = defaults;
      if (dflag != ConfigSaveFlags::NONE) this->save((uint32_t)dflag);
    }
  };

  load("logging.json", m_logging, {{"sink", "baical"}}, ConfigSaveFlags::LOGGING);
  load("graphics.json", m_graphics, {}, ConfigSaveFlags::GRAPHICS);
  load("audio.json", m_audio, {{"volume", 0.5f}, {"device", "[default]"}}, ConfigSaveFlags::AUDIO);
  load("controls.json", m_controls,
       {{"type", "gamepad"},
        {"keybinds",
         {
             {"triangle", ""}, {"square", ""},   {"circle", ""}, {"cross", ""}, {"dpad_up", ""}, {"dpad_down", ""}, {"dpad_left", ""}, {"dpad_right", ""},
             {"options", ""},  {"touchpad", ""}, {"l1", ""},     {"l2", ""},    {"l3", ""},      {"r1", ""},        {"r2", ""},        {"r3", ""},
             {"lx-", ""},      {"lx+", ""},      {"ly-", ""},    {"ly+", ""},   {"rx-", ""},     {"rx+", ""},       {"ry-", ""},       {"ry+", ""},
         }}},
       ConfigSaveFlags::CONTROLS);

  return true;
}

bool Config::save(uint32_t flags) {
  auto save = [this](std::string_view fname, json& j) {
    auto path = std::string("./config/") + fname.data();
    try {
      std::ofstream json_file(path);
      json_file << j;
      return true;
    } catch (const json::exception& e) {
      printf(L"Failed to save %s: %s\n", fname.data(), e.what());
      return false;
    }
  };

  bool result = true;

  if (std::filesystem::is_directory("./config/")) std::filesystem::create_directory("./config/");
  if (flags & (uint32_t)ConfigSaveFlags::LOGGING) result &= save("logging.json", m_logging);
  if (flags & (uint32_t)ConfigSaveFlags::GRAPHICS) result &= save("graphics.json", m_graphics);
  if (flags & (uint32_t)ConfigSaveFlags::AUDIO) result &= save("audio.json", m_audio);

  return true;
}
