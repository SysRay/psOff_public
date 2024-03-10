#pragma once

#include <stdint.h>

namespace Pm4 {

constexpr uint32_t create(uint16_t len, uint16_t baseIndex) {
  return 0x40000000u | (((static_cast<uint16_t>(len) - 2u) & 0x3fffu) << 16u) | baseIndex;
}

#define PM4_GET(u, r, f) (((u) >> Pm4::r##_##f##_SHIFT) & Pm4::r##_##f##_MASK)

constexpr uint32_t R_VS                = 0x01;
constexpr uint32_t R_PS                = 0x02;
constexpr uint32_t R_DRAW_INDEX        = 0x03;
constexpr uint32_t R_DRAW_INDEX_AUTO   = 0x04;
constexpr uint32_t R_DRAW_RESET        = 0x05;
constexpr uint32_t R_WAIT_FLIP_DONE    = 0x06;
constexpr uint32_t R_CS                = 0x07;
constexpr uint32_t R_DISPATCH_DIRECT   = 0x08;
constexpr uint32_t R_DISPATCH_RESET    = 0x09;
constexpr uint32_t R_DISPATCH_WAIT_MEM = 0x0A;
constexpr uint32_t R_PUSH_MARKER       = 0x0B;
constexpr uint32_t R_POP_MARKER        = 0x0C;
constexpr uint32_t R_VS_EMBEDDED       = 0x0D;
constexpr uint32_t R_PS_EMBEDDED       = 0x0E;
constexpr uint32_t R_VS_UPDATE         = 0x0F;
constexpr uint32_t R_PS_UPDATE         = 0x10;
constexpr uint32_t R_VGT_CONTROL       = 0x11;
constexpr uint32_t R_NUM_COMMANDS      = 0x12; // used for array size
} // namespace Pm4