#pragma once
#include <stdint.h>

namespace Err {} // namespace Err

constexpr size_t SCE_NP_SCORE_COMMENT_MAXLEN          = 63;
constexpr size_t SCE_NP_SCORE_GAMEINFO_MAXSIZE        = 189;
constexpr size_t SCE_NP_SCORE_CENSOR_COMMENT_MAXLEN   = 255;
constexpr size_t SCE_NP_SCORE_SANITIZE_COMMENT_MAXLEN = 255;

enum class NpScoreUpdateType { NORMAL, FORCE };

enum class NpScoreOrderType { DESCENDING, ASCENDING };

constexpr uint32_t SCE_NP_SCORE_MAX_RANGE_NUM_PER_REQUEST = 100;
constexpr uint32_t SCE_NP_SCORE_MAX_ID_NUM_PER_REQUEST    = 101;
constexpr uint32_t SCE_NP_SCORE_MAX_SELECTED_FRIENDS_NUM  = 100;

constexpr uint32_t SCE_NP_SCORE_MAX_RANGE_NUM_PER_TRANS  = SCE_NP_SCORE_MAX_RANGE_NUM_PER_REQUEST;
constexpr uint32_t SCE_NP_SCORE_MAX_NPID_NUM_PER_TRANS   = SCE_NP_SCORE_MAX_ID_NUM_PER_REQUEST;
constexpr uint32_t SCE_NP_SCORE_MAX_NPID_NUM_PER_REQUEST = SCE_NP_SCORE_MAX_ID_NUM_PER_REQUEST;

constexpr uint32_t SCE_NP_SCORE_MAX_CTX_NUM = 32;

enum class NpScoreBindMode { ALL_FORBIDDEN = 0x0000, RDONLY = 0x0001, WRONLY = 0x0002, RDWR = (RDONLY | WRONLY), DEFAULT = RDWR };