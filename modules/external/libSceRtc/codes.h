#pragma once

#include <cstdint>

namespace Err {
namespace Rtc {
constexpr int32_t NOT_INITIALIZED     = -2135621631;
constexpr int32_t INVALID_POINTER     = -2135621630;
constexpr int32_t INVALID_VALUE       = -2135621629;
constexpr int32_t INVALID_ARG         = -2135621628;
constexpr int32_t NOT_SUPPORTED       = -2135621627;
constexpr int32_t NO_CLOCK            = -2135621626;
constexpr int32_t BAD_PARSE           = -2135621625;
constexpr int32_t INVALID_YEAR        = -2135621624;
constexpr int32_t INVALID_MONTH       = -2135621623;
constexpr int32_t INVALID_DAY         = -2135621622;
constexpr int32_t INVALID_HOUR        = -2135621621;
constexpr int32_t INVALID_MINUTE      = -2135621620;
constexpr int32_t INVALID_SECOND      = -2135621619;
constexpr int32_t INVALID_MICROSECOND = -2135621618;
} // namespace Rtc
} // namespace Err
