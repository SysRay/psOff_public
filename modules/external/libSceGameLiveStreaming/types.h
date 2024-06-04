#pragma once
#include "codes.h"

enum class SceGameLiveStreamingVideoResolution {
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_640x360   = 0x00000001,
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_960x540   = 0x00000002,
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_1280x720  = 0x00000004,
  SCE_GAME_LIVE_STREAMING_VIDEO_RESOLUTION_1920x1080 = 0x00000008
};

enum class SceGameLiveStreamingCameraFramePosition {
  SCE_GAME_LIVE_STREAMING_CAMERA_FRAME_POSITION_TOP_LEFT     = 0x01,
  SCE_GAME_LIVE_STREAMING_CAMERA_FRAME_POSITION_TOP_RIGHT    = 0x02,
  SCE_GAME_LIVE_STREAMING_CAMERA_FRAME_POSITION_BOTTOM_LEFT  = 0x03,
  SCE_GAME_LIVE_STREAMING_CAMERA_FRAME_POSITION_BOTTOM_RIGHT = 0x04,
};

enum class SceGameLiveStreamingSocialFeedbackMessageType {
  SCE_GAME_LIVE_STREAMING_SOCIAL_FEEDBACK_MESSAGE_TYPE_MESSAGE = 0x01,
  SCE_GAME_LIVE_STREAMING_SOCIAL_FEEDBACK_MESSAGE_TYPE_COMMAND = 0x02,
  SCE_GAME_LIVE_STREAMING_SOCIAL_FEEDBACK_MESSAGE_TYPE_TEXT    = 0x03,
};

typedef int SYSV_ABI (*SceGameLiveStreamingSocialFeedbackMessageFilter)(char* outText, size_t* outTextLength, const char* srcText, const size_t srcTextLength);

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

struct SceGameLiveStreamingCameraFrameSetting {
  SceGameLiveStreamingCameraFramePosition position;
  float                                   alpha;
  bool                                    isDisableCamera;
  uint8_t                                 reserved[31];
};

struct SceGameLiveStreamingBroadcastScreenLayout {
  bool    isCommentListDisplayed;
  uint8_t reserved[31];
};
