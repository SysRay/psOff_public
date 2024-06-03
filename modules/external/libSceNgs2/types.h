#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"
#include "utility/utility.h"

#include <map>
#include <memory>

union SceNgs2VoiceStateFlags {
  uint32_t data;

  struct {
    bool Inuse   : 1 = false;
    bool Playing : 1 = false;
    bool Paused  : 1 = false;
    bool Stopped : 1 = false;
    bool Error   : 1 = false;
    bool Empty   : 1 = true;
  } bits;
};

enum class SceNgs2VoiceEvent : uint32_t {
  Play,
  Stop,
  Stop_imm,
  Kill,
  Pause,
  Resume,
};

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

// Voice events
enum class SceNgs2VoiceParam : uint16_t {
  SET_MATRIX_LEVELS = 1,
  SET_PORT_VOLUME,
  SET_PORT_MATRIX,
  SET_PORT_DELAY,
  PATCH,
  KICK_EVENT,
  SET_CALLBACK,
};

// Master events
enum class SceNgs2MasteringParam : uint16_t {
  SETUP,
  SET_MATRIX,
  SET_LFE,
  SET_LIMITER,
  SET_GAIN,
  SET_OUTPUT,
  SET_PEAK_METER,
};

// sampler events
enum class SceNgs2SamplerParam : uint16_t {
  SETUP,
  ADD_WAVEFORM_BLOCKS,
  REPLACE_WAVEFORM_ADDRESS,
  SET_WAVEFORM_FRAME_OFFSET,
  EXIT_LOOP,
  SET_PITCH,
  SET_ENVELOPE,
  SET_DISTORTION,
  SET_USER_FX,
  SET_PEAK_METER,
  SET_FILTER,
};
// Submixer events
enum class SceNgs2SubmixerParam : uint16_t {
  SETUP,
  SET_ENVELOPE,
  SET_COMPRESSOR,
  SET_DISTORTION,
  SET_USER_FX,
  SET_PEAK_METER,
  SET_FILTER,
};

#pragma pack(push, 1)

struct SceNgs2WaveformInfo {
  SceNgs2WaveFormType  type          = SceNgs2WaveFormType::NONE;
  SceNgs2ChannelsCount channelsCount = SceNgs2ChannelsCount::INVALID;
  uint32_t             sampleRate    = 0;
  uint32_t             configData;
  uint32_t             frameOffset;
  uint32_t             frameMargin;
};

class Reader;

struct SceNgs2VoiceHandle {
  SceNgs2VoiceHandle()  = default;
  ~SceNgs2VoiceHandle() = default;

  SceNgs2VoiceStateFlags state = {0};

  Reader* reader = nullptr;
};

struct SceNgs2ContextBufferInfo {
  void*     hostBuffer;
  size_t    hostBufferSize;
  uintptr_t pad[5];
  void*     userData;
};

struct SceNgs2BufferAllocator {
  int32_t SYSV_ABI (*allocHandler)(SceNgs2ContextBufferInfo*) = nullptr;
  int32_t SYSV_ABI (*freeHandler)(SceNgs2ContextBufferInfo*)  = nullptr;
  void*   userData;
};

struct SceNgs2Handle;

struct SceNgs2RackInfo {
  char                     name[SCE_NGS2_RACK_NAME_LENGTH];
  SceNgs2Handle*           rackHandle;
  SceNgs2ContextBufferInfo bufferInfo;
  SceNgs2Handle*           ownerSystemHandle;
  uint32_t                 type;
  uint32_t                 rackId;
  uint32_t                 uid;
  uint32_t                 minGrainSamples;
  uint32_t                 maxGrainSamples;
  uint32_t                 maxVoices;
  uint32_t                 maxChannelWorks;
  uint32_t                 maxInputs;
  uint32_t                 maxPorts;
  uint32_t                 maxMatrices;
  uint32_t                 stateFlags;
  float                    lastProcessRatio;
  uint64_t                 lastProcessTick;
  uint64_t                 renderCount;
  uint32_t                 activeVoiceCount;
  uint32_t                 activeChannelWorkCount;
};

struct SceNgs2SystemInfo {
  char                     name[SCE_NGS2_SYSTEM_NAME_LENGTH];
  SceNgs2Handle*           systemHandle;
  SceNgs2ContextBufferInfo bufferInfo;
  uint32_t                 uid;
  uint32_t                 minGrainSamples;
  uint32_t                 maxGrainSamples;
  uint32_t                 stateFlags;
  uint32_t                 rackCount;
  float                    lastRenderRatio;
  uint64_t                 lastRenderTick;
  uint64_t                 renderCount;
  uint32_t                 sampleRate;
  uint32_t                 numGrainSamples;
};

struct SceNgs2RackOption {
  size_t size = sizeof(SceNgs2RackOption);

  char     name[SCE_NGS2_RACK_NAME_LENGTH] = "\0";
  uint32_t flags;
  uint32_t maxGrainSamples     = 1;
  uint32_t maxVoices           = 1;
  uint32_t maxInputDelayBlocks = 1;
  uint32_t maxMatrices         = 1;
  uint32_t maxPorts            = 1;

  uint32_t aReserved[20];
};

struct SceNgs2VoiceState {
  uint32_t stateFlags = 0;
};

enum class SceNgs2HandleType { System, Rack, Voice };

struct SceNgs2Handle_rack;

struct SceNgs2Handle {
  SceNgs2HandleType const type;

  SceNgs2Handle(SceNgs2HandleType type): type(type) {}

  virtual ~SceNgs2Handle() = default;
};

struct SceNgs2SystemOption {
  size_t   size;
  char     name[16];
  uint32_t flags;
  uint32_t maxGrainSamples;
  uint32_t numGrainSamples;
  uint32_t sampleRate;
  int8_t   pad[24];
};

struct SceNgs2Handle_system: public SceNgs2Handle {
  SceNgs2BufferAllocator alloc;

  // Racks
  SceNgs2Handle_rack* mastering = nullptr;
  SceNgs2Handle_rack* sampler   = nullptr;
  SceNgs2Handle_rack* submixer  = nullptr;
  SceNgs2Handle_rack* reverb    = nullptr;
  SceNgs2Handle_rack* equalizer = nullptr;

  // -

  // Options
  uint32_t outSampleRate;
  uint32_t outNumSamples;

  // -

  SceNgs2Handle_system(SceNgs2BufferAllocator const* alloc_, const SceNgs2SystemOption* sysopt): SceNgs2Handle(SceNgs2HandleType::System) {
    if (alloc_ != nullptr) alloc = *alloc_;
    outSampleRate = sysopt ? sysopt->sampleRate : 48000;
    outNumSamples = sysopt ? sysopt->numGrainSamples : 256;

    // Handle zero-cases
    outSampleRate = outSampleRate > 0 ? outSampleRate : 48000;
    outNumSamples = outNumSamples > 0 ? outNumSamples : 256;
  }

  virtual ~SceNgs2Handle_system() = default;
};

struct SceNgs2Handle_voice: public SceNgs2Handle {
  SceNgs2Handle_rack* parent;

  SceNgs2VoiceStateFlags state {};
  SceNgs2WaveformInfo    info;

  Reader* reader = nullptr; // optional, depends on racktype

  SceNgs2Handle_voice(SceNgs2Handle_rack* parent): SceNgs2Handle(SceNgs2HandleType::Voice), parent(parent) {}

  virtual ~SceNgs2Handle_voice();

  void ev_KickEvent(SceNgs2VoiceEvent id);
};

struct SceNgs2Handle_rack: public SceNgs2Handle {
  uint32_t const rackId;

  SceNgs2Handle_system* parent;

  SceNgs2RackOption options;

  std::map<int, SceNgs2Handle_voice> voices; // address of SceNgs2Handle_voice must be valid !

  SceNgs2Handle_rack(SceNgs2Handle_system* parent, SceNgs2RackOption const* options_, uint32_t rackId)
      : SceNgs2Handle(SceNgs2HandleType::Rack), rackId(rackId), parent(parent) {
    if (options_ != nullptr) memcpy(&options, options_, sizeof(SceNgs2RackOption));
  }

  virtual ~SceNgs2Handle_rack() = default;
};

struct SceNgs2RenderBufferInfo {
  void*                bufferPtr;
  size_t               bufferSize;
  SceNgs2WaveFormType  waveType;
  SceNgs2ChannelsCount channelsCount;
};

struct SceNgs2WaveformBlock {
  uint32_t offset;
  uint32_t size;
  uint32_t numRepeat;
  uint32_t skipSamples;
  uint32_t numSamples;
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
  uint32_t dataPerFrame;
  uint32_t frameSize;
  uint32_t numframeSamples;
  uint32_t samplesDelay;

  uint32_t             numBlocks;
  SceNgs2WaveformBlock block[4];
};

struct SceNgs2GeomListenerWork;
struct SceNgs2GeomListenerParam;

typedef int  SYSV_ABI (*SceWaveformUserFunc)(uintptr_t ud, uint32_t off, void* data, size_t size);
typedef void SYSV_ABI (*SceNgs2ReportHandler)(const void* data, uintptr_t userData);

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
  SceNgs2Handle*        destHandle;
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

struct SceNgs2CustomSamplerVoiceWaveformAddressParam {
  SceNgs2VoiceParamHead header;
  const void*           pDataStart;
  const void*           pDataEnd;
};

struct SceNgs2SamplerVoiceSetupParam {
  SceNgs2VoiceParamHead header;
  SceNgs2WaveformInfo   format;
  uint32_t              flags;
  uint32_t              reserved;
};

struct SceNgs2SamplerVoiceState {
  SceNgs2VoiceStateFlags voiceState = {0};

  float       envelopeHeight = 0;
  float       peakHeight     = 0;
  uint32_t    reserved;
  uint64_t    numDecodedSamples = 0;
  uint64_t    decodedDataSize   = 0;
  uint64_t    userData          = 0;
  const void* waveformData      = 0;

  SceNgs2SamplerVoiceState() = default;
};

struct SceNgs2VoiceCallbackInfo {
  uintptr_t      callbackData;
  SceNgs2Handle* voiceHandle;
  uint32_t       flag;
  uint32_t       reserved;

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

typedef void SYSV_ABI (*SceNgs2VoiceCallbackHandler)(const SceNgs2VoiceCallbackInfo* info);

struct SceNgs2VoiceCallbackParam {
  SceNgs2VoiceParamHead       header;
  SceNgs2VoiceCallbackHandler callbackHandler;
  uintptr_t                   callbackData;
  uint32_t                    flags;
  uint32_t                    reserved;
};

struct SceNgs2CustomModuleInfo {
  uint32_t moduleId;
  uint32_t sourceBufferId;
  uint32_t extraBufferId;
  uint32_t destBufferId;
  uint32_t stateOffset;
  uint32_t stateSize;
  uint32_t reserved;
  uint32_t reserved2;
};

struct SceNgs2VoiceMatrixInfo {
  uint32_t numLevels;
  float    aLevel[SCE_NGS2_MAX_MATRIX_LEVELS];
};

struct SceNgs2VoicePortInfo {
  int32_t        matrixId;
  float          volume;
  uint32_t       numDelaySamples;
  uint32_t       destInputId;
  SceNgs2Handle* destHandle;
};

struct SceNgs2PanWork {
  float    aSpeakerAngle[SCE_NGS2_MAX_VOICE_CHANNELS];
  float    unitAngle;
  uint32_t numSpeakers;
};

struct SceNgs2PanParam {
  float angle;
  float distance;
  float fbwLevel;
  float lfeLevel;
};

#pragma pack(pop)
