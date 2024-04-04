#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t INVALID_ADDR   = 0x80690001;
constexpr int32_t INVALID_SIZE   = 0x80690002;
constexpr int32_t INVALID_PARAM  = 0x80690003;
constexpr int32_t INVALID_DATA   = 0x80690010;
constexpr int32_t UNSUPPORT_DATA = 0x80690011;
constexpr int32_t DECODE_ERROR   = 0x80690012;
constexpr int32_t FATAL          = 0x80690020;
} // namespace Err
