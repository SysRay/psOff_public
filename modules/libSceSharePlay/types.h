#pragma once
#include "codes.h"
#include "modules/libSceNpManager/types.h"

enum class SceSharePlayProhibitionMode : uint32_t { OFF = 0, CONTROL_ONLY = 1, CONTROL_SCREEN = 2 };

enum class SceSharePlayConnectionStatus : uint32_t {
  DORMANT   = 0,
  READY     = 1,
  CONNECTED = 2,
};

enum class SceSharePlayControllerMode : uint32_t {
  WATCHING_HOST_PLAY     = 0,
  PLAYING_AS_HOST        = 1,
  PLAYING_GAME_WITH_HOST = 2,
};

struct SceSharePlayConnectionInfoA {
  SceSharePlayConnectionStatus status;
  SceSharePlayControllerMode   mode;
  SceNpOnlineId                hostOnelineId;
  SceNpOnlineId                visitorOnlineId;
  SceNpAccountId               hostAccountId;
  SceNpAccountId               visitorAccountId;
  SceUserServiceUserId         hostUserId;
  SceUserServiceUserId         visitorUserId;
};