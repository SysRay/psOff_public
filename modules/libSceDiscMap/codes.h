#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t SCE_DISC_MAP_ERROR_INVALID_ARGUMENT    = -2129657855; // 0x81100001
constexpr int32_t SCE_DISC_MAP_ERROR_LOCATION_NOT_MAPPED = -2129657854; // 0x81100002
constexpr int32_t SCE_DISC_MAP_ERROR_FILE_NOT_FOUND      = -2129657853; // 0x81100003
constexpr int32_t SCE_DISC_MAP_ERROR_NO_BITMAP_INFO      = -2129657852; // 0x81100004
constexpr int32_t SCE_DISC_MAP_ERROR_FATAL               = -2129657601; // 0x811000FF
} // namespace Err

constexpr int32_t DM_PATCH_FLAG = 1;
constexpr int32_t DM_APP1_FLAG  = 0x100;