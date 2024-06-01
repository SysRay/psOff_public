#pragma once

#include <cstdint>

namespace Err {
namespace UserService {
constexpr int32_t INTERNAL                = -2137653247; /* 0x80960001 */
constexpr int32_t NOT_INITIALIZED         = -2137653246; /* 0x80960002 */
constexpr int32_t ALREADY_INITIALIZED     = -2137653245; /* 0x80960003 */
constexpr int32_t NO_MEMORY               = -2137653244; /* 0x80960004 */
constexpr int32_t INVALID_ARGUMENT        = -2137653243; /* 0x80960005 */
constexpr int32_t OPERATION_NOT_SUPPORTED = -2137653242; /* 0x80960006 */
constexpr int32_t NO_EVENT                = -2137653241; /* 0x80960007 */
constexpr int32_t NOT_LOGGED_IN           = -2137653239; /* 0x80960009 */
constexpr int32_t BUFFER_TOO_SHORT        = -2137653238; /* 0x8096000A */
} // namespace UserService
} // namespace Err

constexpr size_t USER_SERVICE_MAX_USER_NAME_LENGTH = 16;
