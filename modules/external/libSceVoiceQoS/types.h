#pragma once
#include "codes.h"

typedef int SceVoiceQoSLocalId;
typedef int SceVoiceQoSRemoteId;
typedef int SceVoiceQoSConnectionId;

enum class SceVoiceQoSAttributeId {
  MIC_VOLUME,
  MIC_MUTE,
  SPEAKER_VOLUME,
  SPEAKER_MUTE,
  DESIRED_OUT_BIT_RATE,
  MIC_USABLE,
  SILENT_STATE,
  REMOTE_MUTE,
  SPEAKER_DESTINATION,

};

enum class SceVoiceQoSStatusId { IN_BITRATE, OUT_BITRATE, OUT_READ_BITRATE, IN_FRAME_RECEIVED_RATIO, HEARTBEAT_FLAG };