#pragma once
#include <stdint.h>

namespace Err {} // namespace Err

constexpr uint32_t SCE_NP_SESSION_INVITATION_EVENT_FLAG_INVITATION = (0x01);
constexpr uint32_t SCE_NP_SESSION_ID_MAX_SIZE                      = (45);
constexpr uint32_t SCE_NP_INVITATION_ID_MAX_SIZE                   = (60);
constexpr uint32_t SCE_NP_MAX_REQUEST_NUM                          = (32);
constexpr uint32_t SCE_NP_TIMEOUT_NO_EFFECT                        = (0);
constexpr uint32_t SCE_NP_POLL_ASYNC_RET_FINISHED                  = (0);
constexpr uint32_t SCE_NP_POLL_ASYNC_RET_RUNNING                   = (1);
constexpr uint32_t SCE_NP_TITLE_ID_LEN                             = (12);
constexpr uint32_t SCE_NP_TITLE_SECRET_SIZE                        = (128);
constexpr uint32_t SCE_NP_CLIENT_ID_MAX_LEN                        = (128);
constexpr uint32_t SCE_NP_CLIENT_SECRET_MAX_LEN                    = (256);
constexpr uint32_t SCE_NP_DEFAULT_SERVICE_LABEL                    = (0x00000000);
constexpr uint32_t SCE_NP_INVALID_SERVICE_LABEL                    = (0xFFFFFFFF);
constexpr uint32_t SCE_NP_AUTHORIZATION_CODE_MAX_LEN               = (128);
constexpr uint32_t SCE_NP_ID_TOKEN_MAX_LEN                         = (4096);
constexpr uint32_t SCE_NP_COMMUNICATION_PASSPHRASE_SIZE            = (128);
constexpr uint32_t SCE_NP_PORT                                     = (3658);
constexpr uint32_t SCE_NP_UNIFIED_ENTITLEMENT_LABEL_SIZE           = (17);
constexpr uint32_t SCE_NP_INVALID_ACCOUNT_ID                       = (0);
constexpr uint32_t SCE_NP_LANGUAGE_CODE_MAX_LEN                    = (5);
constexpr uint32_t SCE_NP_COUNTRY_CODE_LENGTH                      = (2);
constexpr uint32_t SCE_NP_NO_AGE_RESTRICTION                       = (0);
constexpr uint32_t SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY          = (0x1);
constexpr uint32_t SCE_NP_PLUS_EVENT_RECHECK_NEEDED                = (0x1);
constexpr uint32_t SCE_NP_IN_GAME_MESSAGE_DATA_SIZE_MAX            = (512);