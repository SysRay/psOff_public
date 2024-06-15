#pragma once
#include "types.h"

namespace events::system_circle {
class IEventHandler {
  protected:
  IEventHandler();

  public:
  virtual ~IEventHandler() = default;

  virtual void onEventLoadExec(LoadArgs const& data)   = 0;
  virtual void onEventSetArguments(SetArg const& data) = 0;
};
} // namespace events::system_circle
