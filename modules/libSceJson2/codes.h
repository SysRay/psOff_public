#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t PARSE_INVALID_CHAR = -2138799871;
constexpr int32_t NOMEM              = -2138799870;
constexpr int32_t NOFILE             = -2138799869;
constexpr int32_t NOROOT             = -2138799868;
constexpr int32_t NOBUF              = -2138799867;
constexpr int32_t NOINIT             = -2138799856;
constexpr int32_t MULTIPLEINIT       = -2138799855;
constexpr int32_t ALREADY_SET        = -2138799854;
constexpr int32_t NOT_SUPPORTED      = -2138799853;
constexpr int32_t SPECIAL_FLOAT      = -2138799852;
constexpr int32_t NOT_EXIST_KEY      = -2138799851;
constexpr int32_t NOT_EXIST_INDEX    = -2138799850;
constexpr int32_t NOT_A_OBJECT       = -2138799849;
constexpr int32_t NOT_A_CONTAINER    = -2138799848;
constexpr int32_t INVALID_ARGUMENT   = -2138799840;
} // namespace Err
