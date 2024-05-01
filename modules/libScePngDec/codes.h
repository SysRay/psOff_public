#pragma once
#include <stdint.h>

namespace Err {
namespace PngDec {
constexpr int32_t INVALID_ADDR   = -2140602367;
constexpr int32_t INVALID_SIZE   = -2140602366;
constexpr int32_t INVALID_PARAM  = -2140602365;
constexpr int32_t INVALID_HANDLE = -2140602364;
constexpr int32_t INVALID_DATA   = -2140602352;
constexpr int32_t UNSUPPORT_DATA = -2140602351;
constexpr int32_t DECODE_ERROR   = -2140602350;
constexpr int32_t FATAL          = -2140602336;
} // namespace PngDec
} // namespace Err

namespace PngDec {
namespace ImageFlag {
constexpr uint8_t ADAM7_INTERLACE  = 1;
constexpr uint8_t TRNS_CHUNK_EXIST = 2;
} // namespace ImageFlag
}; // namespace PngDec
