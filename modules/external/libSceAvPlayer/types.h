#pragma once
#include "codes.h"
#include "utility/utility.h"
typedef void* SceAvPlayerHandle;

// clang-format off

using SceAvPlayerLogCallback       = SYSV_ABI int (*)(void* userData, const char* format, va_list args);
using SceAvPlayerAllocate          = SYSV_ABI void* (*)(void* argP, uint32_t argAlignment, uint32_t argSize);
using SceAvPlayerDeallocate        = SYSV_ABI void (*)(void* argP, void* argMemory);
using SceAvPlayerAllocateTexture   = SYSV_ABI void* (*)(void* argP, uint32_t argAlignment, uint32_t argSize);
using SceAvPlayerDeallocateTexture = SYSV_ABI void (*)(void* argP, void* argMemory);
using SceAvPlayerOpenFile          = SYSV_ABI int (*)(void* argP, const char* argFilename);
using SceAvPlayerCloseFile         = SYSV_ABI int (*)(void* argP);
using SceAvPlayerReadOffsetFile    = SYSV_ABI int (*)(void* argP, uint8_t* argBuffer, uint64_t argPosition, uint32_t argLength);
using SceAvPlayerEventCallback     = SYSV_ABI void (*)(void* p, int32_t argEventId, int32_t argSourceId, void* argEventData);
using SceAvPlayerSizeFile          = SYSV_ABI uint64_t (*)(void* argP);

// clang-format on

enum class SceAvPlayerVideoDecoderType { DEFAULT = 0, RESERVED1, RESERVED2, SOFTWARE2 };

enum class SceAvPlayerAudioDecoderType { DEFAULT = 0, RESERVED1, RESERVED2 };

enum class SceAvPlayerAudioChannelOrder { DEFAULT = 0, EXTL_EXTR_LS_RS, LS_RS_EXTL_EXTR };

enum SceAvPlayerStreamType { VIDEO, AUDIO, TIMEDTEXT, UNKNOWN };

enum class SceAvPlayerEvents {
  STATE_STOP      = 0x01,
  STATE_READY     = 0x02,
  STATE_PLAY      = 0x03,
  STATE_PAUSE     = 0x04,
  STATE_BUFFERING = 0x05,

  TIMED_TEXT_DELIVERY = 0x10,
  WARNING_ID          = 0x20,
  ENCRYPTION          = 0x30,
  DRM_ERROR           = 0x40
};

enum class SceAvPlayerTrickSpeeds { NORMAL = 100, FF_4X = 400, FF_8X = 800, FF_16X = 1600, FF_MAX = 3200, RW_8X = -800, RW_16X = -1600, RW_MAX = -3200 };

enum class SceAvPlayerAvSyncMode {
  DEFAULT = 0,

  NONE
};

enum class SceAvPlayerDebuglevels { NONE, INFO, WARNINGS, ALL };

enum class SceAvPlayerDRMType { MARLIN, PLAYREADY };

struct SceAvPlayerDecoderInit {
  union {
    SceAvPlayerVideoDecoderType videoType;
    SceAvPlayerAudioDecoderType audioType;
    uint8_t                     reserved[4];
  } decoderType;

  union {
    struct {
      int32_t cpuAffinityMask;

      int32_t cpuThreadPriority;

      uint8_t decodePipelineDepth;

      uint8_t computePipeId;
      uint8_t computeQueueId;
      uint8_t enableInterlaced;

      uint8_t reserved[16];
    } avcSw2;

    struct {
      uint8_t audioChannelOrder;
      uint8_t reserved[27];
    } aac;

    uint8_t reserved[28];
  } decoderParams;
};

struct SceAvPlayerHTTPCtx {
  uint32_t httpCtxId;
  uint32_t sslCtxId;
};

struct SceAvPlayerPostInitData {
  uint32_t               demuxVideoBufferSize;
  SceAvPlayerDecoderInit videoDecoderInit;
  SceAvPlayerDecoderInit audioDecoderInit;
  SceAvPlayerHTTPCtx     httpCtx;
};

struct SceAvPlayerDRMError {
  SceAvPlayerDRMType type;
  int32_t            errorCode;
};

struct SceAvPlayerAudio {
  uint16_t channelCount;
  uint8_t  reserved1[2];
  uint32_t sampleRate;
  uint32_t size;

  uint8_t languageCode[4];
};

struct SceAvPlayerVideo {
  uint32_t width;
  uint32_t height;
  float    aspectRatio;
  uint8_t  languageCode[4];
};

struct SceAvPlayerTextPosition {
  uint16_t top;
  uint16_t left;
  uint16_t bottom;
  uint16_t right;
};

struct SceAvPlayerTimedText {
  uint8_t                 languageCode[4];
  uint16_t                textSize;
  uint16_t                fontSize;
  SceAvPlayerTextPosition position;
};

union SceAvPlayerStreamDetails {
  uint8_t              reserved[16];
  SceAvPlayerAudio     audio;
  SceAvPlayerVideo     video;
  SceAvPlayerTimedText subs;
};

struct SceAvPlayerStreamInfo {
  uint32_t                 type;
  uint8_t                  reserved[4];
  SceAvPlayerStreamDetails details;
  uint64_t                 duration;
  uint64_t                 startTime;
};

struct SceAvPlayerFrameInfo {
  uint8_t*                 pData;
  uint8_t                  reserved[4];
  uint64_t                 timeStamp;
  SceAvPlayerStreamDetails details;
};

struct SceAvPlayerMemAllocator {
  void* objectPointer;

  SceAvPlayerAllocate          allocate;
  SceAvPlayerDeallocate        deallocate;
  SceAvPlayerAllocateTexture   allocateTexture;
  SceAvPlayerDeallocateTexture deallocateTexture;
};

struct SceAvPlayerFileReplacement {
  void* objectPointer;

  SceAvPlayerOpenFile       open;
  SceAvPlayerCloseFile      close;
  SceAvPlayerReadOffsetFile readOffset;
  SceAvPlayerSizeFile       size;
};

struct SceAvPlayerEventReplacement {
  void* objectPointer;

  SceAvPlayerEventCallback eventCallback;
};

struct SceAvPlayerInitData {
  SceAvPlayerMemAllocator memoryReplacement;

  SceAvPlayerFileReplacement  fileReplacement;
  SceAvPlayerEventReplacement eventReplacement;
  SceAvPlayerDebuglevels      debugLevel;
  uint32_t                    basePriority;

  int32_t numOutputVideoFrameBuffers;

  bool        autoStart;
  uint8_t     reserved[3];
  const char* defaultLanguage;
};
