#pragma once
#include "codes.h"

enum class SceVoicePortType {
  PORTTYPE_NULL         = -1,
  PORTTYPE_IN_DEVICE    = 0,
  PORTTYPE_IN_PCMAUDIO  = 1,
  PORTTYPE_IN_VOICE     = 2,
  PORTTYPE_OUT_PCMAUDIO = 3,
  PORTTYPE_OUT_VOICE    = 4,
  PORTTYPE_OUT_DEVICE   = 5,
};

enum class SceVoiceBitRate {
  BITRATE_NULL  = -1,
  BITRATE_3850  = 3850,
  BITRATE_4650  = 4650,
  BITRATE_5700  = 5700,
  BITRATE_7300  = 7300,
  BITRATE_14400 = 14400,
  BITRATE_16000 = 16000,
  BITRATE_22533 = 22533,
};

enum class SceVoicePcmDataType {
  PCM_NULL                = -1,
  PCM_SHORT_LITTLE_ENDIAN = 0,
};

enum class SceVoiceSamplingRate {
  SAMPLINGRATE_NULL  = -1,
  SAMPLINGRATE_16000 = 16000,
};

enum class SceVoicePortState {
  PORTSTATE_NULL      = -1,
  PORTSTATE_IDLE      = 0,
  PORTSTATE_READY     = 1,
  PORTSTATE_BUFFERING = 2,
  PORTSTATE_RUNNING   = 3,
};

struct SceVoicePCMFormat {
  SceVoicePcmDataType  dataType;
  SceVoiceSamplingRate sampleRate;
};

struct SceVoicePortParam {
  SceVoicePortType portType;
  uint16_t         threshold;
  uint16_t         bMute;
  float            volume;

  union {
    struct {
      SceVoiceBitRate bitrate;
    } voice;

    struct {
      uint32_t          bufSize;
      SceVoicePCMFormat format;
    } pcmaudio;

    struct {
      int32_t type, userId;
      int32_t index;
    } device;
  };
};

struct SceVoiceBasePortInfo {
  SceVoicePortType  portType;
  SceVoicePortState state;
  uint32_t*         pEdge;
  uint32_t          numByte;
  uint32_t          frameSize;
  uint16_t          numEdge;
  uint16_t          reserved;
};

struct SceVoiceResourceInfo {
  uint16_t maxInVoicePort;
  uint16_t maxOutVoicePort;
  uint16_t maxInDevicePort;
  uint16_t maxOutDevicePort;
};