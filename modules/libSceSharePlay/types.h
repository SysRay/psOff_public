#pragma once
#include "codes.h"
#include "modules/libSceNpManager/types.h"

enum class SceSharePlayProhibitionMode : uint32_t {
  SCE_SHARE_PLAY_PROHIBITION_MODE_OFF = 0,
  SCE_SHARE_PLAY_PROHIBITION_MODE_CONTROL_ONLY = 1,
  SCE_SHARE_PLAY_PROHIBITION_MODE_CONTROL_SCREEN = 2
};

enum class SceSharePlayConnectionStatus : uint32_t {
  SCE_SHARE_PLAY_CONNECTION_STATUS_DORMANT = 0,
  SCE_SHARE_PLAY_CONNECTION_STATUS_READY = 1,
  SCE_SHARE_PLAY_CONNECTION_STATUS_CONNECTED = 2,
};

enum class SceSharePlayControllerMode : uint32_t {
  SCE_SHARE_PLAY_CONTROLLER_MODE_WATCHING_HOST_PLAY = 0,
  SCE_SHARE_PLAY_CONTROLLER_MODE_PLAYING_AS_HOST = 1,
  SCE_SHARE_PLAY_CONTROLLER_MODE_PLAYING_GAME_WITH_HOST = 2,
};

struct SceSharePlayConnectionInfoA {
  SceSharePlayConnectionStatus status;
  SceSharePlayControllerMode mode;
  SceNpOnlineId  hostOnelineId;
  SceNpOnlineId  visitorOnlineId;
  SceNpAccountId hostAccountId;
  SceNpAccountId visitorAccountId;
  SceUserServiceUserId hostUserId;
  SceUserServiceUserId visitorUserId;
};