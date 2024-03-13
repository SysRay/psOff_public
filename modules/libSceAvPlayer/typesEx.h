#pragma once
#include "types.h"

enum class SceAvPlayerUriType { SOURCE = 0 };

enum class SceAvPlayerSourceType { UNKNOWN = 0, FILE_MP4 = 1, HLS = 8 };

enum class SceAvPlayerPriorityType {
  INHERIT               = (0),
  SYSTEM_DEFAULT        = 700,
  OFFSET_DEMUX          = (2),
  OFFSET_VIDEO_DECODER  = (5),
  OFFSET_AUDIO_DECODER  = (6),
  OFFSET_CONTROLLER     = (9),
  OFFSET_HTTP_STREAMING = (10),
  OFFSET_FILE_STREAMING = (15)
};

enum class SceAvPlayerAffinityType {
  AFFINITY_INHERIT               = (0),
  AFFINITY_1                     = (1),
  AFFINITY_2                     = (1 << 1),
  AFFINITY_3                     = (1 << 2),
  AFFINITY_4                     = (1 << 3),
  AFFINITY_5                     = (1 << 4),
  AFFINITY_6                     = (1 << 5),
  AFFINITY_SYSTEM_7_CPU          = (1 << 6),
  AFFINITY_SYSTEM_DEFAULT        = (1) | (1 << 1) | (1 << 2) | (1 << 3),
  AFFINITY_SYSTEM_FIRST_CLUSTER  = (1) | (1 << 1) | (1 << 2) | (1 << 3),
  AFFINITY_SYSTEM_SECOND_CLUSTER = (1 << 4) | (1 << 5),
  AFFINITY_SYSTEM_ALL_CLUSTER    = (1) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5),
};

struct SceAvPlayerAudioEx {
  uint16_t channelCount;
  uint8_t  reserved[2];
  uint32_t sampleRate;
  uint32_t size;
  uint8_t  languageCode[4];
  uint8_t  reserved1[64];
};

struct SceAvPlayerVideoEx {
  uint32_t width;
  uint32_t height;
  float    aspectRatio;
  uint8_t  languageCode[4];
  uint32_t framerate;
  uint32_t cropLeftOffset;
  uint32_t cropRightOffset;
  uint32_t cropTopOffset;
  uint32_t cropBottomOffset;
  uint32_t pitch;
  uint8_t  lumaBitDepth;
  uint8_t  chromaBitDepth;
  bool     videoFullRangeFlag;

  uint8_t reserved1[37];
};

struct SceAvPlayerTimedTextEx {
  uint8_t languageCode[4];
  uint8_t reserved[12];
  uint8_t reserved1[64];
};

union SceAvPlayerStreamDetailsEx {
  SceAvPlayerAudioEx     audio;
  SceAvPlayerVideoEx     video;
  SceAvPlayerTimedTextEx subs;
};

struct SceAvPlayerFrameInfoEx {
  void*                      pData;
  uint8_t                    reserved[4];
  uint64_t                   timeStamp;
  SceAvPlayerStreamDetailsEx details;
};

struct SceAvPlayerUri {
  const char* name;
  uint32_t    length;
};

struct SceAvPlayerSourceDetails {
  SceAvPlayerUri        uri;
  uint8_t               reserved[64];
  SceAvPlayerSourceType sourceType;
};

struct SceAvPlayerInitDataEx {
  size_t                      thisSize;
  SceAvPlayerMemAllocator     memoryReplacement;
  SceAvPlayerFileReplacement  fileReplacement;
  SceAvPlayerEventReplacement eventReplacement;
  const char*                 defaultLanguage;

  SceAvPlayerDebuglevels debugLevel;
  uint32_t               audioDecoderPriority;
  uint32_t               audioDecoderAffinity;
  uint32_t               videoDecoderPriority;

  uint32_t videoDecoderAffinity;

  uint32_t demuxerPriority;
  uint32_t demuxerAffinity;
  uint32_t controllerPriority;
  uint32_t controllerAffinity;
  uint32_t httpStreamingPriority;
  uint32_t httpStreamingAffinity;
  uint32_t fileStreamingPriority;
  uint32_t fileStreamingAffinity;
  int32_t  numOutputVideoFrameBuffers;

  bool    autoStart;
  uint8_t reserved[3];

  uint32_t audioDecoderStackSize;
  uint32_t videoDecoderStackSize;
  uint32_t demuxerStackSize;
  uint32_t controllerStackSize;
  uint32_t httpStreamingStackSize;

  uint32_t fileStreamingStackSize;
};