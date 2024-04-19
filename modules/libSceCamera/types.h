#pragma once
#include "codes.h"
#include "modules_include/sceTypes.h"

enum class SceCameraConfigType {
  CONFIG_TYPE1     = 0x01,
	CONFIG_TYPE2     = 0x02,
	CONFIG_TYPE3     = 0x03,
	CONFIG_TYPE4     = 0x04,
	CONFIG_TYPE5     = 0x05,
	CONFIG_EXTENTION = 0x10,
};

enum class SceCameraBaseFormat {
  BASE_FORMAT_YUV422  = 0x0,
	BASE_FORMAT_NO_USE  = 0x10,
	BASE_FORMAT_UNKNOWN = 0xFF,
};

enum class SceCameraScaleFormat {
  SCALE_FORMAT_YUV422  = 0x0,
	SCALE_FORMAT_Y16     = 0x3,
	SCALE_FORMAT_NO_USE  = 0x10,
	SCALE_FORMAT_UNKNOWN = 0xFF,
};

enum class SceCameraResolution {
  RESOLUTION_1280X800               = 0x0,
	RESOLUTION_640X400                = 0x1,
	RESOLUTION_320X200                = 0x2,
	RESOLUTION_160X100                = 0x3,
	RESOLUTION_320X192                = 0x4,
	RESOLUTION_SPECIFIED_WIDTH_HEIGHT = 0x5,
	RESOLUTION_UNKNOWN                = 0xFF,
};

enum class SceCameraFramerate {
  FRAMERATE_UNKNOWN = 0,
	FRAMERATE_7_5     = 7,
	FRAMERATE_1       = 15,
	FRAMERATE_30      = 30,
	FRAMERATE_60      = 60,
	FRAMERATE_120     = 120,
	FRAMERATE_240     = 240,
};

enum class SceCameraChannel {
 	CHANNEL_0    = 1,
	CHANNEL_1    = 2,
	CHANNEL_BOTH = 3,
};

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

struct SceCameraFormat {
  SceCameraBaseFormat  formatLevel0;
	SceCameraScaleFormat formatLevel1;
	SceCameraScaleFormat formatLevel2;
	SceCameraScaleFormat formatLevel3;
};

struct SceCameraConfigExtention {
  SceCameraFormat     format;
	SceCameraResolution resolution;
	SceCameraFramerate  framerate;
	uint32_t            width;
	uint32_t            height;
	uint32_t            reserved1; 
	void*               pBaseOption;
};

struct SceCameraConfig {
  uint32_t                 sizeThis;
	SceCameraConfigType      configType;
	SceCameraConfigExtention configExtention[SCE_CAMERA_MAX_DEVICE_NUM];
};

struct SceCameraVideoSyncParameter {
  uint32_t sizeThis;
	uint32_t videoSyncMode;
	void*    pModeOption;
};

struct SceCameraStartParameter {
  uint32_t sizeThis;
	uint32_t formatLevel[SCE_CAMERA_MAX_DEVICE_NUM];
	void*    pStartOption;
};

struct SceCameraAttribute {
  uint32_t               sizeThis;
	SceCameraChannel       channel;
	SceCameraFramePosition framePosition;
	SceCameraExposureGain  exposureGain;
	SceCameraWhiteBalance  whiteBalance;
	SceCameraGamma         gamma;
	uint32_t               saturation;
	uint32_t               contrast;
	uint32_t               sharpness;
	int32_t                hue;
	uint32_t               reserved1;
	uint32_t               reserved2;
	uint32_t               reserved3;
	uint32_t               reserved4;
};