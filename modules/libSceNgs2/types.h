#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"

enum class SceNgs2WaveFormType : uint32_t {
  NONE = 0,

  PCM_I8 = 0x10,
  PCM_U8 = 0x11,

  PCM_I16LITTLE = 0x12,
  PCM_I16BIG    = 0x13,

  PCM_I24LITTLE = 0x14,
  PCM_I24BIG    = 0x15,

  PCM_I32LITTLE = 0x16,
  PCM_I32BIG    = 0x17,

  PCM_F32LITTLE = 0x18,
  PCM_F32BIG    = 0x19,

  PCM_F64LITTLE = 0x1A,
  PCM_F64BIG    = 0x1B,

  VAG = 0x1C,

  ATRAC9 = 0x40,

  MAX_TYPES

  /* More? */
};

enum class SceNgs2ChannelsCount : uint32_t {
  CH_1_0 = 1,
  CH_2_0 = 2,
  CH_5_1 = 6,
  CH_7_1 = 8,

  INVALID = 0xFFFFFFFF
};

enum class SceNgs2VoiceParam : uint32_t {
  SET_MATRIX_LEVELS = 1,
  SET_PORT_VOLUME,
  SET_PORT_MATRIX,
  SET_PORT_DELAY,
  PATCH,
  KICK_EVENT,
  SET_CALLBACK,
};

struct SceNgs2ContextBufferInfo {
  void*     hostBuffer;
  size_t    hostBufferSize;
  uintptr_t pad[5];
  void*     userData;
};

struct SceNgs2BufferAllocator {
  int32_t (*allocHandler)(SceNgs2ContextBufferInfo*);
  int32_t (*freeHandler)(SceNgs2ContextBufferInfo*);
  void* userData;
};

struct SceNgs2Handle { // todo: split this type? Or use union inside it.
  SceNgs2Handle*           owner;
  SceNgs2BufferAllocator   alloc;
  SceNgs2ContextBufferInfo cbi;
};

struct SceNgs2RenderBufferInfo {
  void*                bufferPtr;
  size_t               bufferSize;
  SceNgs2WaveFormType  waveType;
  SceNgs2ChannelsCount channelsCount;
};

struct SceNgs2WaveformInfo {
  SceNgs2WaveFormType  type;
  SceNgs2ChannelsCount channelsCount;
  uint32_t             sampleRate;
  uint32_t             configData;
  uint32_t             frameOffset;
  uint32_t             frameMargin;
};

struct SceNgs2WaveformBlock {
  uint32_t offset;
  uint32_t size;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uintptr_t userData;
};

struct SceNgs2WaveformFormat {
  SceNgs2WaveformInfo info;
  uint32_t            offset;
  uint32_t            size;
  uint32_t            loopBeginPos;
  uint32_t            loopEndPos;
  uint32_t            samplesCount;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t             numBlocks;
  SceNgs2WaveformBlock block[4];
};

struct SceNgs2SystemOption {
  size_t   size;
  char     name[16];
  uint32_t flags;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t sampleRate;
  int8_t   pad[24];
};

struct SceNgs2GeomListenerWork;
struct SceNgs2GeomListenerParam;

typedef int (*SceWaveformUserFunc)(uintptr_t ud, uint32_t off, void* data, size_t size);

struct SceNgs2GeomVector {
  float x, y, z;
};

struct SceNgs2GeomSourceParam {
  SceNgs2GeomVector pos;
  SceNgs2GeomVector vel;
  SceNgs2GeomVector dir;
  /* todo */
};

struct SceNgs2VoiceParamHead {
  uint16_t size;
  int16_t  next;
  uint32_t id;
};

struct SceNgs2VoiceMatrixLevelsParam {
  SceNgs2VoiceParamHead header;
  uint32_t              matrixId;
  uint32_t              numLevels;
  const float*          aLevel;
};

struct SceNgs2VoicePortMatrixParam {
  SceNgs2VoiceParamHead header;
  uint32_t              port;
  int32_t               matrixId;
};

struct SceNgs2VoicePortVolumeParam {
  SceNgs2VoiceParamHead header;
  uint32_t              port;
  float                 level;
};

struct SceNgs2VoicePortDelayParam {
  SceNgs2VoiceParamHead header;
  uint32_t              port;
  uint32_t              numSamples;
};

struct SceNgs2VoicePatchParam {
  SceNgs2VoiceParamHead header;
  uint32_t              port;
  uint32_t              destInputId;
  SceNgs2Handle         destHandle;
};

struct SceNgs2VoiceEventParam {
  SceNgs2VoiceParamHead header;
  uint32_t              eventId;
};

struct SceNgs2SamplerVoiceWaveformBlocksParam {
  SceNgs2VoiceParamHead       header;
  const void*                 data;
  uint32_t                    flags;
  uint32_t                    numBlocks;
  const SceNgs2WaveformBlock* aBlock;
};

struct SceNgs2VoiceCallbackInfo {
  uintptr_t     callbackData;
  SceNgs2Handle voiceHandle;
  uint32_t      flag;
  uint32_t      reserved;

  union {
    struct {
      uintptr_t   userData;
      const void* data;
      uint32_t    dataSize;
      uint32_t    repeatedCount;
      uint32_t    attributeFlags;
      uint32_t    reserved2;
    } waveformBlock;
  } param;
};

typedef void (*SceNgs2VoiceCallbackHandler)(const SceNgs2VoiceCallbackInfo* info);

struct SceNgs2VoiceCallbackParam {
  SceNgs2VoiceParamHead       header;
  SceNgs2VoiceCallbackHandler callbackHandler;
  uintptr_t                   callbackData;
  uint32_t                    flags;
  uint32_t                    reserved;
};

struct SceNgs2VoiceState {
  uint32_t stateFlags;
};

struct SceNgs2RackOption {
  size_t   size;
  char     name[SCE_NGS2_RACK_NAME_LENGTH];
  uint32_t flags;
  uint32_t maxGrainSamples;
  uint32_t maxVoices;
  uint32_t maxInputDelayBlocks;
  uint32_t maxMatrices;
  uint32_t maxPorts;
  uint32_t aReserved[20];
};
