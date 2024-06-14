#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t ERROR_NOT_INIT                 = -2131886079; // 0x80EE0001
constexpr int32_t ERROR_ALREADY_INIT             = -2131886078; // 0x80EE0002
constexpr int32_t ERROR_INVALID_ARG              = -2131886077; // 0x80EE0003
constexpr int32_t ERROR_INVALID_HANDLE           = -2131886076; // 0x80EE0004
constexpr int32_t ERROR_MAX_CONTROLLERS_EXCEEDED = -2131886075; // 0x80EE0005
constexpr int32_t ERROR_INVALID_PORT             = -2131886074; // 0x80EE0006
constexpr int32_t ERROR_ALREADY_OPENED           = -2131886073; // 0x80EE0007
constexpr int32_t ERROR_FATAL                    = -2131885825; // 0x80EE00FF
} // namespace Err

constexpr uint8_t SCE_MOVE_MAX_CONTROLLERS            = 4; ///< Maximum number of motion controllers.
constexpr uint8_t SCE_MOVE_MAX_CONTROLLERS_PER_PLAYER = 2; ///< Maximum number of motion controllers that can be bound to a player.
