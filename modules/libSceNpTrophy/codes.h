#pragma once
#include <stdint.h>

namespace Err {
namespace NpTrophy {
constexpr int32_t EXCEEDS_MAX         = -2141907422;
constexpr int32_t ALREADY_EXISTS      = -2141907437;
constexpr int32_t INVALID_CONTEXT     = -2141907447;
constexpr int32_t INVALID_ARGUMENT    = -2141907452;
constexpr int32_t SCREENSHOT_DISABLED = -2141907413;
} // namespace NpTrophy
} // namespace Err

namespace NpTrophy {
constexpr int32_t INVALID_HANDLE     = -1;
constexpr int32_t INVALID_CONTEXT    = -1;
constexpr int32_t INVALID_TROPHY_ID  = -1;
constexpr int32_t INVALID_GROUP_ID   = -2;
constexpr int32_t BASE_GAME_GROUP_ID = -1;

constexpr uint32_t NUM_MAX                   = (128);
constexpr uint32_t SCREENSHOT_TARGET_NUM_MAX = (4);
constexpr uint32_t GAME_TITLE_MAX_SIZE       = (128);
constexpr uint32_t GAME_DESCR_MAX_SIZE       = (1024);
constexpr uint32_t GROUP_TITLE_MAX_SIZE      = (128);
constexpr uint32_t GROUP_DESCR_MAX_SIZE      = (1024);
constexpr uint32_t NAME_MAX_SIZE             = (128);
constexpr uint32_t DESCR_MAX_SIZE            = (1024);
} // namespace NpTrophy
