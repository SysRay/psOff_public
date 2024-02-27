#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t ERROR_INVALID_ARG          = -2132869119;
constexpr int32_t ERROR_INVALID_PORT         = -2132869118;
constexpr int32_t ERROR_INVALID_HANDLE       = -2132869117;
constexpr int32_t ERROR_ALREADY_OPENED       = -2132869116;
constexpr int32_t ERROR_NOT_INITIALIZED      = -2132869115;
constexpr int32_t ERROR_DEVICE_NOT_CONNECTED = -2132869113;
constexpr int32_t ERROR_FATAL                = -2132868865;
} // namespace Err

constexpr uint8_t SCE_MOUSE_PORT_TYPE_STANDARD      = 0; /* standard mouse */
constexpr uint8_t SCE_MOUSE_MAX_STANDARD_PORT_INDEX = 2; /* max index of standard mouse */