#pragma once
#include "third_party/nlohmann/json.hpp"
#include "utility/utility.h"

#include <boost/thread/mutex.hpp>

using json = nlohmann::json;

enum class ConfigModFlag : uint32_t {
  NONE     = 0,
  LOGGING  = 2 << 0,
  GRAPHICS = 2 << 1,
  AUDIO    = 2 << 2,
  CONTROLS = 2 << 3,
  GENERAL  = 2 << 4,
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

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IConfig* accessConfig();
#undef __APICALL
