#pragma once
#include <stdint.h>

namespace Err {
constexpr int VALIDATION_NOT_ENABLED = 0x80D13FFF;
constexpr int FAILURE                = 0x800EEEFF;
constexpr int RAZOR_NOT_LOADED       = 0x80D15001;
} // namespace Err