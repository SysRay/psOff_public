#pragma once
#include <stdint.h>

namespace Err {
namespace DiscMap {
constexpr int32_t INVALID_ARGUMENT    = -2129657855; // 0x81100001
constexpr int32_t LOCATION_NOT_MAPPED = -2129657854; // 0x81100002
constexpr int32_t FILE_NOT_FOUND      = -2129657853; // 0x81100003
constexpr int32_t NO_BITMAP_INFO      = -2129657852; // 0x81100004
constexpr int32_t FATAL               = -2129657601; // 0x811000FF
} // namespace DiscMap
} // namespace Err

constexpr int32_t DM_PATCH_FLAG = 1;
constexpr int32_t DM_APP1_FLAG  = 0x100;
