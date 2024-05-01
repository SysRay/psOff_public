#pragma once

#include <stdint.h>

namespace Pm4 {

#define PM4_GET(u, r, f) (((u) >> Pm4::r##_##f##_SHIFT) & Pm4::r##_##f##_MASK)

enum class Custom : uint16_t {
  R_VS,
  R_PS,
  R_DRAW_INDEX,
  R_DRAW_INDEX_AUTO,
  R_DRAW_INDEX_OFFSET,
  R_DRAW_RESET,
  R_WAIT_FLIP_DONE,
  R_CS,
  R_DISPATCH_DIRECT,
  R_DISPATCH_RESET,
  R_DISPATCH_WAIT_MEM,
  R_PUSH_MARKER,
  R_POP_MARKER,
  R_VS_EMBEDDED,
  R_PS_EMBEDDED,
  R_VS_UPDATE,
  R_PS_UPDATE,
  R_VGT_CONTROL,
  R_NUM_COMMANDS, // used for array size
};

constexpr uint32_t create(uint16_t len, Custom baseIndex) {
  return 0x40000000u | (((static_cast<uint16_t>(len) - 2u) & 0x3fffu) << 16u) | (uint16_t)baseIndex;
}

} // namespace Pm4