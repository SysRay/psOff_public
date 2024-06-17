#pragma once
#include "codes.h"

enum class ScePngDecColorSpace : uint16_t {
  GRAYSCALE       = 2,
  RGB             = 3,
  CLUT            = 4,
  GRAYSCALE_ALPHA = 18,
  RGBA            = 19,

  INVALID = 0xFFFF,
};

enum class ScePngDecPixelFormat : uint16_t {
  R8G8B8A8 = 0,
  B8G8R8A8 = 1,
};

typedef void* ScePngDecHandle;

struct ScePngDecParseParam {
  const void* pngAddr;
  uint32_t    pngSize;
  uint32_t    reserved;
};

struct ScePngDecImageInfo {
  uint32_t            imageWidth;
  uint32_t            imageHeight;
  ScePngDecColorSpace colorSpace;
  uint16_t            bitDepth;
  uint32_t            imageFlag;
};

struct ScePngDecCreateParam {
  uint32_t cbSize;
  uint32_t attribute;
  uint32_t maxImageWidth;
};

struct ScePngDecDecodeParam {
  const void*          pngAddr;
  void*                imageAddr;
  uint32_t             pngSize;
  uint32_t             imageSize;
  ScePngDecPixelFormat pixelFormat;
  uint16_t             alphaValue;
  uint32_t             imagePitch;
};
