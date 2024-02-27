#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t RESOLVER_TIMEOUT    = (0);
constexpr int32_t RESOLVER_RETRY      = (0);
constexpr int32_t CONNECT_TIMEOUT     = (30 * 1000 * 1000U);
constexpr int32_t SEND_TIMEOUT        = (120 * 1000 * 1000U);
constexpr int32_t RECV_TIMEOUT        = (120 * 1000 * 1000U);
constexpr int32_t RECV_BLOCK_SIZE     = (1500U);
constexpr int32_t RESPONSE_HEADER_MAX = (5000U);
constexpr int32_t REDIRECT_MAX        = (6U);
constexpr int32_t TRY_AUTH_MAX        = (6U);
} // namespace Err

constexpr uint32_t SCE_HTTP_TRUE              = (int)(1);
constexpr uint32_t SCE_HTTP_FALSE             = (int)(0);
constexpr uint32_t SCE_HTTP_ENABLE            = SCE_HTTP_TRUE;
constexpr uint32_t SCE_HTTP_DISABLE           = SCE_HTTP_FALSE;
constexpr uint32_t SCE_HTTP_USERNAME_MAX_SIZE = 256;
constexpr uint32_t SCE_HTTP_PASSWORD_MAX_SIZE = 256;