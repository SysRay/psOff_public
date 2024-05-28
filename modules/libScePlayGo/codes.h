#pragma once
#include <stdint.h>

namespace Err {
namespace PlayGo {
constexpr int32_t UNKNOWN             = -2135818239; /* 0x80B20001 */
constexpr int32_t FATAL               = -2135818238; /* 0x80B20002 */
constexpr int32_t NO_MEMORY           = -2135818237; /* 0x80B20003 */
constexpr int32_t INVALID_ARGUMENT    = -2135818236; /* 0x80B20004 */
constexpr int32_t NOT_INITIALIZED     = -2135818235; /* 0x80B20005 */
constexpr int32_t ALREADY_INITIALIZED = -2135818234; /* 0x80B20006 */
constexpr int32_t ALREADY_STARTED     = -2135818233; /* 0x80B20007 */
constexpr int32_t NOT_STARTED         = -2135818232; /* 0x80B20008 */
constexpr int32_t BAD_HANDLE          = -2135818231; /* 0x80B20009 */
constexpr int32_t BAD_POINTER         = -2135818230; /* 0x80B2000A */
constexpr int32_t BAD_SIZE            = -2135818229; /* 0x80B2000B */
constexpr int32_t BAD_CHUNK_ID        = -2135818228; /* 0x80B2000C */
constexpr int32_t BAD_SPEED           = -2135818227; /* 0x80B2000D */
constexpr int32_t NOT_SUPPORT_PLAYGO  = -2135818226; /* 0x80B2000E */
constexpr int32_t EPERMISSION         = -2135818225; /* 0x80B2000F */
constexpr int32_t BAD_LOCUS           = -2135818224; /* 0x80B20010 */
constexpr int32_t NEED_DATA_DISC      = -2135818223; /* 0x80B20011 */
} // namespace PlayGo
} // namespace Err
