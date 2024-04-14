#pragma once
#include "codes.h"

enum class SceAudioOutChannel { L = 0, R, C, LFE, LS, RS, LE, RE, MAX };

enum class SceAudioOutChannelOrderStd { L = 0, R, C, LFE, LE, RE, LS, RS, MAX };

struct SceAudioOutOutputParam {
  int32_t     handle;
  const void* ptr;
};

enum class SceAudioOutPortType {
  MAIN     = 0,  // Main Audio
  BGM      = 1,  // Background Music
  VOICE    = 2,  // Voice
  PERSONAL = 3,  // Personal Audio
  PADSPK   = 4,  // Pad Speaker
  AUX      = 127 // Auxiliary Audio

};

enum class SceAudioOutParamFormat : unsigned int {
  S16_MONO      = 0,
  S16_STEREO    = 1,
  S16_8CH       = 2,
  FLOAT_MONO    = 3,
  FLOAT_STEREO  = 4,
  FLOAT_8CH     = 5,
  S16_8CH_STD   = 6,
  FLOAT_8CH_STD = 7

};

enum class SceAudioOutStateOutput {
  UNKNOWN             = 0,
  CONNECTED_PRIMARY   = (1 << 0),
  CONNECTED_SECONDARY = (1 << 1),
  CONNECTED_TERTIARY  = (1 << 2),
  CONNECTED_HEADPHONE = (1 << 6),
  CONNECTED_EXTERNAL  = (1 << 7),
};

enum class SceAudioOutStateChannel {
  CHANNEL_UNKNOWN      = 0,
  CHANNEL_DISCONNECTED = 0,
  CHANNEL_1            = 1,
  CHANNEL_2            = 2,
  CHANNEL_6            = 6,
  CHANNEL_8            = 8,
};

enum class SceAudioOutStateFlag {
  NONE = 0,
};

struct SceAudioOutPortState {
  uint16_t output;         // SceAudioOutStateOutput (bitwise OR)
  uint8_t  channel;        // SceAudioOutStateChannel
  uint8_t  reserved8_1[1]; // reserved
  int16_t  volume;
  uint16_t rerouteCounter;
  uint64_t flag; // SceAudioOutStateFlag (bitwise OR)
};

struct SceAudioOutSystemState {
  float    loudness;
  uint8_t  reserved8[4];
  uint64_t reserved64[3];
};

enum class SceAudioOutSystemDebugStateElement { NULL_ = 0, LOUDNESS, MAX };

enum SceAudioOutSystemDebugStateLoudness { LOUDNESS_STOP = 0, LOUDNESS_START, LOUDNESS_START_QUIET, LOUDNESS_MAX };

struct SceAudioOutSystemDebugStateParam {
  uint32_t value;
  uint32_t reserved[3]; // reserved
};
