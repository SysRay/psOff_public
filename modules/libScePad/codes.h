#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t INVALID_ARG              = -2137915391; /* 0x80920001 */
constexpr int32_t INVALID_PORT             = -2137915390; /* 0x80920002 */
constexpr int32_t INVALID_HANDLE           = -2137915389; /* 0x80920003 */
constexpr int32_t ALREADY_OPENED           = -2137915388; /* 0x80920004 */
constexpr int32_t NOT_INITIALIZED          = -2137915387; /* 0x80920005 */
constexpr int32_t INVALID_LIGHTBAR_SETTING = -2137915386; /* 0x80920006 */
constexpr int32_t DEVICE_NOT_CONNECTED     = -2137915385; /* 0x80920007 */
constexpr int32_t NO_HANDLE                = -2137915384; /* 0x80920008 */
constexpr int32_t FATAL                    = -2137915137; /* 0x809200FF */

} // namespace Err

constexpr uint32_t SCE_PAD_MAX_TOUCH_NUM               = 2;
constexpr uint32_t SCE_PAD_MAX_DEVICE_UNIQUE_DATA_SIZE = 12;