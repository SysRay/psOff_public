#pragma once
#include "codes.h"

typedef void* ScePngDecHandle;

struct ScePngDecParseParam {
  const void* pngMemAddr;
  uint32_t    pngMemSize;
  uint32_t    reserved0;
};

struct ScePngDecImageInfo {
  uint32_t imageWidth;
  uint32_t imageHeight;
  uint16_t colorSpace;
  uint16_t bitDepth;
  uint32_t imageFlag;
};

struct ScePngDecCreateParam {
  uint32_t thisSize;
  uint32_t attribute;
  uint32_t maxImageWidth;
};

struct ScePngDecDecodeParam {
  const void* pngAddr;
  void*       imageAddr;
  uint32_t    pngSize;
  uint32_t    imageSize;
  uint16_t    pixelFormat;
  uint16_t    alphaValue;
  uint32_t    imagePitch;
};
