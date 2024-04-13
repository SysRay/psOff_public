#pragma once
#include "codes.h"

enum class ViewCrop {
  VR_VIEW_CROP_VERTICAL_OFFSET_MIN     = 0,
  VR_VIEW_CROP_VERTICAL_OFFSET_NEUTRAL = 0,
  VR_VIEW_CROP_VERTICAL_OFFSET_MAX     = 28,
};

enum class SceVideoOutCursorPendingType {
  IMAGE_ADDRESS_PENDING = 0,
  DISABLE_PENDING       = 1,
};

enum class SceVideoOutResolutionStatusFlags {
  OUTPUT_MASK       = (1 << 0),
  OUTPUT_IN_USE     = (0 << 0),
  OUTPUT_NOT_IN_USE = (1 << 0),
};

enum class SceVideoOutBusType {
  BUS_TYPE_MAIN = 0,
};

enum class SceVideoOutPixelFormat : uint32_t {
  PIXEL_FORMAT_A8R8G8B8_SRGB            = 0x80000000,
  PIXEL_FORMAT_B8_G8_R8_A8_SRGB         = PIXEL_FORMAT_A8R8G8B8_SRGB,
  PIXEL_FORMAT_A16R16G16B16_FLOAT       = 0xC1060000,
  PIXEL_FORMAT_B16_G16_R16_A16_FLOAT    = PIXEL_FORMAT_A16R16G16B16_FLOAT,
  PIXEL_FORMAT_A8B8G8R8_SRGB            = 0x80002200,
  PIXEL_FORMAT_R8_G8_B8_A8_SRGB         = PIXEL_FORMAT_A8B8G8R8_SRGB,
  PIXEL_FORMAT_A2R10G10B10              = 0x88060000,
  PIXEL_FORMAT_B10_G10_R10_A2           = PIXEL_FORMAT_A2R10G10B10,
  PIXEL_FORMAT_A2R10G10B10_SRGB         = 0x88000000,
  PIXEL_FORMAT_B10_G10_R10_A2_SRGB      = PIXEL_FORMAT_A2R10G10B10_SRGB,
  PIXEL_FORMAT_A2R10G10B10_BT2020_PQ    = 0x88740000,
  PIXEL_FORMAT_B10_G10_R10_A2_BT2020_PQ = PIXEL_FORMAT_A2R10G10B10_BT2020_PQ,
};

enum class SceVideoOutFlipMode {
  VSYNC         = 1,
  HSYNC         = 2,
  WINDOW        = 3,
  VSYNC_MULTI   = 4,
  VSYNC_MULTI_2 = 5,
  WINDOW_2      = 6,
};

enum class SceVideoOutTilingMode {
  TILE   = 0,
  LINEAR = 1,
};

enum class SceVideoOutAspectRatio {
  VBLANK_START = 2,
};

struct SceVideoOutStereoBuffers {
  void* left;
  void* right;
};

struct SceVideoOutBufferAttribute {
  SceVideoOutPixelFormat pixelFormat;
  int32_t                tilingMode;
  int32_t                aspectRatio;
  uint32_t               width;
  uint32_t               height;
  uint32_t               pitchInPixel;
  uint32_t               option;
  uint32_t               _reserved0;
  uint64_t               _reserved1;
};

struct SceVideoOutFlipStatus {
  uint64_t count;
  uint64_t processTime;
  uint64_t tsc;
  int64_t  flipArg;
  uint64_t submitTsc;
  uint64_t _reserved0;
  int32_t  gcQueueNum     = 0;
  int32_t  flipPendingNum = 0;
  int32_t  currentBuffer;
  uint32_t _reserved1;
};

struct SceVideoOutVblankStatus {
  uint64_t count;
  uint64_t processTime;
  uint64_t tsc;
  uint64_t _reserved[1];
  uint8_t  flags;
  uint8_t  pad1[7];
};

struct SceVideoOutResolutionStatus {
  uint32_t fullWidth;
  uint32_t fullHeight;
  uint32_t paneWidth;
  uint32_t paneHeight;
  uint64_t refreshRate = 0;
  float    screenSizeInInch;
  uint16_t flags = 0;
  uint16_t _reserved0;
};

struct SceVideoOutColorSettings {
  float    gamma[3];
  uint32_t option;
};

struct SceVideoOutDeviceCapabilityInfo {
  uint64_t capability; // SceVideoOutDeviceCapability
};

struct SceVideoOutMode {
  uint32_t size = sizeof(SceVideoOutMode);
  uint8_t  signalEncoding;
  uint8_t  signalRange;
  uint8_t  colorimetry;
  uint8_t  depth;
  uint64_t refreshRate;
  uint64_t resolution;
  uint8_t  contentType;
  uint8_t  _reserved0[3];
  uint32_t _reserved[1];
};

struct SceVideoOutVrViewCropAdjustment {
  int16_t verticalOffset;
  int16_t reserved0;
  int32_t reserved1[3];
};

struct SceVideoOutConfigureOptions {
  SceVideoOutVrViewCropAdjustment vrViewCropAdjustment;
};
