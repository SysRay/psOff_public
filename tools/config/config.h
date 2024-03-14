#pragma once
#include "third_party/nlohmann/json.hpp"
#include "utility/utility.h"

#include <boost/thread/mutex.hpp>

class IConfig {
  CLASS_NO_COPY(IConfig);
  CLASS_NO_MOVE(IConfig);

  protected:
  IConfig() = default;

  public:
  virtual ~IConfig() = default;

  virtual std::pair<boost::unique_lock<boost::mutex>, nlohmann::json*> accessLogging() = 0;
};

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IConfig* accessConfig();
#undef __APICALL