#pragma once
#include "codes.h"

enum class SceRTCDay {
  SUNDAY    = 0,
  MONDAY    = 1,
  TUESDAY   = 2,
  WEDNESDAY = 3,
  THURSDAY  = 4,
  FRIDAY    = 5,
  SATURDAY  = 6,
};

struct SceRtcDateTime {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
  uint32_t microsecond;
};