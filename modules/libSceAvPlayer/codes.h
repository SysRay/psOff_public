#pragma once
#include <stdint.h>

namespace Err {
namespace AvPlayer {
constexpr int32_t INVALID_PARAMS          = -2140536831; /* 0x806A0001 */
constexpr int32_t OPERATION_FAILED        = -2140536830; /* 0x806A0002 */
constexpr int32_t NO_MEMORY               = -2140536829; /* 0x806A0003 */
constexpr int32_t NOT_SUPPORTED           = -2140536828; /* 0x806A0004 */
constexpr int32_t WAR_FILE_NONINTERLEAVED = -2140536672; /* 0x806A00A0 */
constexpr int32_t WAR_LOOPING_BACK        = -2140536671; /* 0x806A00A1 */
constexpr int32_t WAR_JUMP_COMPLETE       = -2140536669; /* 0x806A00A3 */
constexpr int32_t INFO_MARLIN_ENCRY       = -2140536656; /* 0x806A00B0 */
constexpr int32_t INFO_PLAYREADY_ENCRY    = -2140536652; /* 0x806A00B4 */
constexpr int32_t INFO_AES_ENCRY          = -2140536651; /* 0x806A00B5 */
constexpr int32_t INFO_OTHER_ENCRY        = -2140536641; /* 0x806A00BF */
} // namespace AvPlayer
} // namespace Err
