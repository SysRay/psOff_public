#pragma once
#include "codes.h"
#include "modules_include/sceTypes.h"

struct SceCameraFramePosition {
  uint32_t x;
  uint32_t y;
  uint32_t xSize;
  uint32_t ySize;
};

struct SceCameraExposureGain {
  uint32_t exposureControl;
  uint32_t exposure;
  uint32_t gain;
  uint32_t mode;
};

struct SceCameraWhiteBalance {
  uint32_t whiteBalanceControl;
  uint32_t gainRed;
  uint32_t gainBlue;
  uint32_t gainGreen;
};

struct SceCameraGamma {
  uint32_t gammaControl;
  uint32_t value;
  uint8_t  reserved[16];
};

struct SceCameraMeta {
  uint32_t              metaMode;
  uint32_t              format[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
  uint64_t              frame[SCE_CAMERA_MAX_DEVICE_NUM];
  uint64_t              timestamp[SCE_CAMERA_MAX_DEVICE_NUM];
  uint32_t              deviceTimestamp[SCE_CAMERA_MAX_DEVICE_NUM];
  SceCameraExposureGain exposureGain[SCE_CAMERA_MAX_DEVICE_NUM];
  SceCameraWhiteBalance whiteBalance[SCE_CAMERA_MAX_DEVICE_NUM];
  SceCameraGamma        gamma[SCE_CAMERA_MAX_DEVICE_NUM];
  uint32_t              luminance[SCE_CAMERA_MAX_DEVICE_NUM];
  SceFVector3           acceleration;
  uint64_t              vcounter;
  uint32_t              reserved[16];
};

struct SceCameraFrameData {
  uint32_t               sizeThis;
  uint32_t               readMode;
  SceCameraFramePosition framePosition[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
  void*                  pFramePointerList[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
  uint32_t               frameSize[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
  uint32_t               status[SCE_CAMERA_MAX_DEVICE_NUM];
  SceCameraMeta          meta;
  void*                  pFramePointerListGarlic[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
};

struct SceCameraOpenParam {
  uint32_t sizeThis;
  uint32_t reserved1;
  uint32_t reserved2;
  uint32_t reserved3;
};
