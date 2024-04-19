#pragma once
#include <stdint.h>

namespace Err {
namespace NetCtl {
constexpr int32_t NOT_INITIALIZED  = -2143215359;
constexpr int32_t CALLBACK_MAX     = -2143215357;
constexpr int32_t ID_NOT_FOUND     = -2143215356;
constexpr int32_t INVALID_ID       = -2143215355;
constexpr int32_t INVALID_CODE     = -2143215354;
constexpr int32_t INVALID_ADDR     = -2143215353;
constexpr int32_t NOT_CONNECTED    = -2143215352;
constexpr int32_t NOT_AVAIL        = -2143215351;
constexpr int32_t NETWORK_DISABLED = -2143215347;
constexpr int32_t DISCONNECT_REQ   = -2143215346;
constexpr int32_t INVALID_TYPE     = -2143215345;
constexpr int32_t INVALID_SIZE     = -2143215343;
constexpr int32_t ETHERNET_PLUGOUT = -2143215339;
constexpr int32_t WIFI_DEAUTHED    = -2143215338;
constexpr int32_t WIFI_BEACON_LOST = -2143215337;
} // namespace Ctl
} // namespace Err

constexpr size_t SCE_NET_CTL_SSID_LEN              = 33;
constexpr size_t SCE_NET_CTL_WIFI_SECURITY_KEY_LEN = 65;
constexpr size_t SCE_NET_CTL_AUTH_NAME_LEN         = 128;
constexpr size_t SCE_NET_CTL_AUTH_KEY_LEN          = 128;
constexpr size_t SCE_NET_CTL_HOSTNAME_LEN          = 256;
constexpr size_t SCE_NET_CTL_IPV4_ADDR_STR_LEN     = 16;
