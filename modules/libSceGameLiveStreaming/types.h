#pragma once
#include "codes.h"

enum class SceGameLiveStreamingVideoResolution {
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_640x360   = 0x00000001,
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_960x540   = 0x00000002,
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_1280x720  = 0x00000004,
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_1920x1080 = 0x00000008
};

struct SceGameLiveStreamingStatus {
  bool                 isOnAir;
  uint32_t             spectatorCounts;
  SceUserServiceUserId userId;
  uint8_t              reserved[60];
};

struct SceGameLiveStreamingStatus2 {
  SceUserServiceUserId                userId;
  bool                                isOnAir;
  uint32_t                            spectatorCounts;
  uint32_t                            textMessageCounts;
  uint32_t                            commandMessageCounts;
  SceGameLiveStreamingVideoResolution broadcastVideoResolution;
  uint8_t                             reserved[48];
};

struct SceGameLiveStreamingGuardArea {
  float _flt[4];
};
