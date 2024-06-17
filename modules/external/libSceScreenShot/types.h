#pragma once
#include "codes.h"

enum class SceScreenShotOrigin {
  kLeftTop      = 1,
  kLeftCenter   = 2,
  kLeftBottom   = 3,
  kCenterTop    = 4,
  kCenterCenter = 5,
  kCenterBottom = 6,
  kRightTop     = 7,
  kRightCenter  = 8,
  kRightBottom  = 9
};

struct SceScreenShotParam {
  uint32_t    thisSize;
  const char* photoTitle;
  const char* gameTitle;
  const char* gameComment;
  void*       reserved;
};