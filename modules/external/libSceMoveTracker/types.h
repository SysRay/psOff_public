#pragma once
#include "codes.h"

struct SceMoveButtonData {
  uint16_t digitalButtons;
  uint16_t analogT;
};

struct SceMoveExtensionPortData {};

struct SceMoveData {
  float                    accelerometer[3];
  float                    gyro[3];
  SceMoveButtonData        pad;
  SceMoveExtensionPortData ext;
  int64_t                  timestamp;
  int                      counter;
  float                    temperature;
};

struct SceMoveTrackerControllerInput {
  int32_t      handle;
  SceMoveData* data;
  int32_t      num;
};
