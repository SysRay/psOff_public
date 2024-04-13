#pragma once
#include "third_party/nlohmann/json.hpp"
#include "utility/utility.h"

#include <boost/thread/mutex.hpp>

using json = nlohmann::json;

enum class ConfigModFlag : uint32_t {
  NONE     = 0,
  LOGGING  = 1 << 0,
  GRAPHICS = 1 << 1,
  AUDIO    = 1 << 2,
  CONTROLS = 1 << 3,
  GENERAL  = 1 << 4,
  RESOLVE  = 1 << 5,
};

class IConfig {
  CLASS_NO_COPY(IConfig);
  CLASS_NO_MOVE(IConfig);

  protected:
  IConfig() = default;

  public:
  virtual ~IConfig() = default;

  virtual std::pair<boost::unique_lock<boost::mutex>, json*> accessModule(ConfigModFlag) = 0;

  virtual bool save(uint32_t flags) = 0;
};

template <typename T>
bool getJsonParam(json* jData, std::string_view param, T& ret) {
  try {
    (*jData)[param.data()].get_to(ret);
  } catch (...) {
    return false;
  }
  return true;
}

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IConfig* accessConfig();
#undef __APICALL
