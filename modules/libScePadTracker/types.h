#pragma once
#include "codes.h"

enum class ScePadTrackerStatus {
  TRACKER_TRACKING      = 0,
  TRACKER_NOT_TRACKING  = 1,
  TRACKER_ROOM_CONFLICT = 2,
  TRACKER_CALIBRATING   = 3,
};

struct ScePadTrackerImageCoordinates {
  ScePadTrackerStatus status;
  float               x;
  float               y;
};

struct ScePadTrackerImage {
  int   exposure;
  int   gain;
  int   width;
  int   height;
  void* data; 
};

struct ScePadTrackerInput {
  int32_t            handles[TRACKER_CONTROLLER_MAX];
  ScePadTrackerImage images[TRACKER_IMAGE_MAX];
};

struct ScePadTrackerData {
  ScePadTrackerImageCoordinates 
  imageCoordinates[TRACKER_IMAGE_MAX];
};