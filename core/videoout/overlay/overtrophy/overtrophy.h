#pragma once

#include "utility/utility.h"

class IOverTrophy {
  CLASS_NO_COPY(IOverTrophy);
  CLASS_NO_MOVE(IOverTrophy);

  public:
  IOverTrophy()  = default;
  ~IOverTrophy() = default;

  virtual void draw(double fps) = 0;
};

IOverTrophy& accessTrophyOverlay();
