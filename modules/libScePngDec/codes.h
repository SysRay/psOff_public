#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t INVALID_ADDR   = -2140602367;
constexpr int32_t INVALID_SIZE   = -2140602366;
constexpr int32_t INVALID_PARAM  = -2140602365;
constexpr int32_t INVALID_HANDLE = -2140602364;
constexpr int32_t INVALID_DATA   = -2140602352;
constexpr int32_t UNSUPPORT_DATA = -2140602351;
constexpr int32_t DECODE_ERROR   = -2140602350;
constexpr int32_t FATAL          = -2140602336;
} // namespace Err

enum class ScePngDecColorSpace : uint16_t {
  GRAYSCALE       = 2,
  RGB             = 3,
  CLUT            = 4,
  GRAYSCALE_ALPHA = 18,
  RGBA            = 19,
};

enum class ScePngDecPixelFormat : uint16_t {
  R8G8B8A8 = 0,
  B8G8R8A8 = 1,
};

constexpr uint8_t SCE_PNG_DEC_IMAGE_FLAG_ADAM7_INTERLACE  = 1;
constexpr uint8_t SCE_PNG_DEC_IMAGE_FLAG_TRNS_CHUNK_EXIST = 2;
