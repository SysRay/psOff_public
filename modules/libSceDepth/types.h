#pragma once
#include "codes.h"

enum class SceDepthPixelFormat {
  PIXEL_FORMAT_Y8     = 0,
  PIXEL_FORMAT_YUV422 = 1,
};

enum class SceDepthProfile {
  PROFILE_10 = 0,
  PROFILE_20 = 3,
  PROFILE_16 = 4,
};

enum class SceDepthStereoCameraType {
  STEREO_CAMERA_TYPE_PLAYSTATION_CAMERA = 0,
};

enum class SceDepthContextType {
  CONTEXT_TYPE_GFX     = 0,
  CONTEXT_TYPE_COMPUTE = 1,
};

struct SceDepthInputImageInformation {
  int                 width;
	int                 height;
	SceDepthPixelFormat pixelFormat;
};

struct SceDepthProcessingInformation {
  int                      width;
	int                      height;
	int                      executionMode;
	int                      updateMode;
	SceDepthProfile          depthProfile;
	int                      depthMapSizeType;
	SceDepthStereoCameraType cameraType;
};

struct SceDepthPlatformInformation {
  SceDepthContextType contextType;
	int                 pipe;
	int                 queue;
	void*               device;
	void*               context;
};

struct SceDepthInitializeParameter {
  int                           sizeofInitializeParameter;
	SceDepthInputImageInformation inputImageInformation;
	SceDepthProcessingInformation processingInformation;
	SceDepthPlatformInformation   platformInformation;
};

struct SceDepthMemoryInformation {
  void*    systemMemory;
	uint32_t systemMemorySize;
	uint32_t reserved0;
	void*    systemSharedMemory;
	uint32_t systemSharedMemorySize;
	uint32_t reserved1;
	void*    videoSharedMemory;
	uint32_t videoSharedMemorySize;
	uint32_t reserved2;
};

struct SceDepthQueryMemoryResult {
  uint32_t systemMemorySize;
	uint32_t systemSharedMemorySize;
	uint32_t videoSharedMemorySize;
};
