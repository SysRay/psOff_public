#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t SCE_NP_TROPHY_INVALID_HANDLE     = (-1);
constexpr int32_t SCE_NP_TROPHY_INVALID_CONTEXT    = (-1);
constexpr int32_t SCE_NP_TROPHY_INVALID_TROPHY_ID  = (-1);
constexpr int32_t SCE_NP_TROPHY_INVALID_GROUP_ID   = (-2);
constexpr int32_t SCE_NP_TROPHY_BASE_GAME_GROUP_ID = (-1);
} // namespace Err

constexpr uint32_t SCE_NP_TROPHY_NUM_MAX                   = (128);
constexpr uint32_t SCE_NP_TROPHY_SCREENSHOT_TARGET_NUM_MAX = (4);
constexpr uint32_t SCE_NP_TROPHY_GAME_TITLE_MAX_SIZE       = (128);
constexpr uint32_t SCE_NP_TROPHY_GAME_DESCR_MAX_SIZE       = (1024);
constexpr uint32_t SCE_NP_TROPHY_GROUP_TITLE_MAX_SIZE      = (128);
constexpr uint32_t SCE_NP_TROPHY_GROUP_DESCR_MAX_SIZE      = (1024);
constexpr uint32_t SCE_NP_TROPHY_NAME_MAX_SIZE             = (128);
constexpr uint32_t SCE_NP_TROPHY_DESCR_MAX_SIZE            = (1024);
