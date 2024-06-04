#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t SERVICE_ERROR_INTERNAL                        = -2136932351; /* 0x80A10001 */
constexpr int32_t SERVICE_ERROR_UNAVAILABLE                     = -2136932350; /* 0x80A10002 */
constexpr int32_t SERVICE_ERROR_PARAMETER                       = -2136932349; /* 0x80A10003 */
constexpr int32_t SERVICE_ERROR_NO_EVENT                        = -2136932348; /* 0x80A10004 */
constexpr int32_t SERVICE_ERROR_REJECTED                        = -2136932347; /* 0x80A10005 */
constexpr int32_t SERVICE_ERROR_NEED_DISPLAY_SAFE_AREA_SETTINGS = -2136932346; /* 0x80A10006 */
constexpr int32_t SERVICE_ERROR_INVALID_URI_LEN                 = -2136932345; /* 0x80A10007 */
constexpr int32_t SERVICE_ERROR_INVALID_URI_SCHEME              = -2136932344; /* 0x80A10008 */
constexpr int32_t SERVICE_ERROR_NO_APP_INFO                     = -2136932343; /* 0x80A10009 */
constexpr int32_t SERVICE_ERROR_NOT_FLAG_IN_PARAM_SFO           = -2136932342; /* 0x80A1000A */
} // namespace Err

enum class SceSystemServiceParamId : int32_t {
  Lang                = 1,
  DateFormat          = 2,
  TimeFormat          = 3,
  TimeZone            = 4,
  Summertime          = 5,
  SystemName          = 6,
  GameParentalLevel   = 7,
  ENTER_BUTTON_ASSIGN = 1000
};

enum class SceSystemServiceEventType : int32_t {
  INVALID                           = -1,
  ON_RESUME                         = 0x10000000,
  GAME_LIVE_STREAMING_STATUS_UPDATE = 0x10000001,
  SESSION_INVITATION                = 0x10000002,
  ENTITLEMENT_UPDATE                = 0x10000003,
  GAME_CUSTOM_DATA                  = 0x10000004,
  DISPLAY_SAFE_AREA_UPDATE          = 0x10000005,
  URL_OPEN                          = 0x10000006,
  LAUNCH_APP                        = 0x10000007,
  APP_LAUNCH_LINK                   = 0x10000008,
  ADDCONTENT_INSTALL                = 0x10000009,
  RESET_VR_POSITION                 = 0x1000000a,
  JOIN_EVENT                        = 0x1000000b,
  PLAYGO_LOCUS_UPDATE               = 0x1000000c,
  PLAY_TOGETHER_HOST                = 0x1000000d,
  SERVICE_ENTITLEMENT_UPDATE        = 0x1000000e,
  EYE_TO_EYE_DISTANCE_UPDATE        = 0x1000000f,
  JOIN_MATCH_EVENT                  = 0x10000010,
  PLAY_TOGETHER_HOST_A              = 0x10000011,
  WEBBROWSER_CLOSED                 = 0x10000012,
  CONTROLLER_SETTINGS_CLOSED        = 0x10000013,
  JOIN_TEAM_ON_TEAM_MATCH_EVENT     = 0x10000014,
  JOIN_FFA_MATCH_EVENT              = 0x10000015,
  JOIN_FFA_TEAM_MATCH_EVENT         = 0x10000016,
  GAME_INTENT                       = 0x10000017,
  OPEN_SHARE_MENU                   = 0x30000000,
  UNIFIED_ENTITLEMENT_UPDATE        = 0x10000018,
};

enum class SceSystemServiceGpuLoadEmulationMode {
  OFF,
  NORMAL,
  HIGH,
};

constexpr int32_t SCE_SYSTEM_SERVICE_MAX_SYSTEM_NAME_LENGTH = 65;
