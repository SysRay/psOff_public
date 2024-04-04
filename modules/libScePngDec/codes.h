#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t INVALID_ADDR   = 0x80690001;
constexpr int32_t INVALID_SIZE   = 0x80690002;
constexpr int32_t INVALID_PARAM  = 0x80690003;
constexpr int32_t INVALID_HANDLE = 0x80690004;
constexpr int32_t INVALID_DATA   = 0x80690010;
constexpr int32_t UNSUPPORT_DATA = 0x80690011;
constexpr int32_t DECODE_ERROR   = 0x80690012;
constexpr int32_t FATAL          = 0x80690020;
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
