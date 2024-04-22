#pragma once
#include <stdint.h>

namespace Err {
namespace Ult {
constexpr int32_t ERROR_NULL    = -2139029503;
constexpr int32_t ERROR_INVALID = -2139029500;
constexpr int32_t ERROR_BUSY    = -2139029497;
constexpr int32_t ERROR_RANGE   = -2139029501;
} // namespace Ult
} // namespace Err

#define SCE_ULT_MAX_NAME_LENGTH 32
