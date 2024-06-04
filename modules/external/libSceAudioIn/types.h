#pragma once
#include "codes.h"

enum class SceAudioInParamFormat : unsigned int {
  S16_MONO   = 0, // monaural
  S16_STEREO = 2  // stereo

};

enum class SceAudioInSilentState { DEVICE_NONE = 0x00000001, PRIORITY_LOW = 0x00000002, USER_SETTING = 0x00000004, UNABLE_FORMAT = 0x00000008 };

enum class SceAudioInType { VOICE_CHAT = 0, GENERAL = 1, VOICE_RECOGNITION = 5 };