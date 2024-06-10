#pragma once
#include "types.h"

namespace events::system {
class IEventHandler {
  protected:
  IEventHandler();

  public:
  virtual ~IEventHandler() = default;

  virtual void onLoadExec(LoadExecData const& data) = 0;
};
} // namespace events::system
