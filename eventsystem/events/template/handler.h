#pragma once
#include "types.h"

namespace events::video {
class IEventHandler {
  protected:
  IEventHandler();

  public:
  virtual ~IEventHandler() = default;

  virtual void onEventFlip()                         = 0;
  virtual void onEventVBlank(VBlankData const& data) = 0;
};
} // namespace events::video