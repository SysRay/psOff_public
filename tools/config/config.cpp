#define __APICALL_EXTERN
#include "config.h"
#undef __APICALL_EXTERN

class Config: public IConfig {
  nlohmann::json m_logging;
  boost::mutex   m_mutex_logging;

  public:
  Config()          = default;
  virtual ~Config() = default;

  std::pair<boost::unique_lock<boost::mutex>, nlohmann::json*> accessLogging() final;
};

IConfig* accessConfig() {
  static Config obj;
  return &obj;
}

std::pair<boost::unique_lock<boost::mutex>, nlohmann::json*> Config::accessLogging() {
  boost::unique_lock lock(m_mutex_logging);
  return std::make_pair(std::move(lock), &m_logging);
}