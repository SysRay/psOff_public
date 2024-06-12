#pragma once
#include "types.h"

namespace events::system {
class IEventHandler {
  protected:
  IEventHandler();

  public:
  virtual ~IEventHandler() = default;

  virtual void onEventLoadExec(LoadArgs const& data)   = 0;
  virtual void onEventSetArguments(SetArg const& data) = 0;
  virtual void onEventRunExec()                        = 0;
};
} // namespace events::system
