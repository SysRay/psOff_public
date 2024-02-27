#pragma once
#include <stdint.h>
#include <string>

namespace Err {} // namespace Err

constexpr std::string_view SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8 = "application/json; charset=utf-8";

constexpr uint32_t SCE_NP_WEBAPI_DEFAULT_CONNECTION_NUM                = 1;
constexpr uint32_t SCE_NP_WEBAPI_MAX_CONNECTION_NUM                    = 16;
constexpr uint32_t SCE_NP_WEBAPI_PUSH_EVENT_DATA_TYPE_LEN_MAX          = 64;
constexpr uint32_t SCE_NP_WEBAPI_EXTD_PUSH_EVENT_EXTD_DATA_KEY_LEN_MAX = 32;
constexpr uint32_t SCE_NP_WEBAPI_NP_SERVICE_NAME_NONE                  = NULL;