#pragma once
#include "third_party/nlohmann/json.hpp"
#include "utility/utility.h"

#include <boost/thread/mutex.hpp>

enum class ConfigSaveFlags : uint32_t { NONE = 0, LOGGING = 2 << 0, GRAPHICS = 2 << 1, AUDIO = 2 << 2, CONTROLS = 2 << 3 };

class IConfig {
  CLASS_NO_COPY(IConfig);
  CLASS_NO_MOVE(IConfig);

  protected:
  IConfig() = default;

  public:
  virtual ~IConfig() = default;

  virtual std::pair<boost::unique_lock<boost::mutex>, nlohmann::json*> accessModule(ConfigSaveFlags) = 0;

  virtual bool load()               = 0;
  virtual bool save(uint32_t flags) = 0;
};

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IConfig* accessConfig();
#undef __APICALL
