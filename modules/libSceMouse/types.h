#pragma once
#include "codes.h"

enum class MouseButton : uint32_t {
  PRIMARY     = 0x00000001,
  SECONDARY   = 0x00000002,
  OPTIONAL    = 0x00000004,
  OPTIONAL2   = 0x00000008,
  OPTIONAL3   = 0x00000010,
  INTERCEPTED = 0x80000000

};

typedef struct SceMouseData {
  uint64_t timestamp = 0;     /*E System timestamp of this data(micro seconds). */
  bool     connected = false; /*E Mouse connection status. true:connected  false:removed */
  uint32_t buttons   = 0;     /*E Mouse buttons information */
  int32_t  xAxis     = 0;     /*E Amount of movement in the x direction */
  int32_t  yAxis     = 0;     /*E Amount of movement in the y direction */
  int32_t  wheel     = 0;     /*E Amount of wheel movement */
  int32_t  tilt      = 0;     /*E Amount of tilt movement (only for a mouse that supports a tilt wheel) */
  uint8_t  reserve[8];        /*E Reserved */
} SceMouseData;

typedef struct SceMouseOpenParam {
  uint8_t behaviorFlag; /*E Behavior flag */
  uint8_t reserve[7];   /*E Reserved */
} SceMouseOpenParam;