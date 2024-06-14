#pragma once
#include "codes.h"

enum class ScePngEncColorSpace : uint16_t {
  RGB  = 3,
  RGBA = 19,

  INVALID = 0xFFFF,
};

enum class ScePngEncPixelFormat : uint16_t {
  R8G8B8A8 = 0,
  B8G8R8A8 = 1,
};

typedef void* ScePngEncHandle;

struct ScePngEncCreateParam {
  uint32_t cbSize;
  uint32_t attribute;
  uint32_t maxImageWidth;
  uint32_t maxFilterNumber;
};

struct ScePngEncEncodeParam {
  const void* imageAddr;
  void*       pngAddr;
  uint32_t    imageSize;
  uint32_t    pngSize;
  uint32_t    imageWidth;
  uint32_t    imageHeight;
  uint32_t    imagePitch;
  uint16_t    pixelFormat;
  uint16_t    colorSpace;
  uint16_t    bitDepth;
  uint16_t    clutNumber;
  uint16_t    filterType;
  uint16_t    compressionLevel;
};

struct ScePngEncOutputInfo {
  uint32_t dataSize;
  uint32_t processedHeight;
};
