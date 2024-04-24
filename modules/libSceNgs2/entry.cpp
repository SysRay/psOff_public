#include "codes.h"
#include "common.h"
extern "C" {
#include "libavformat/avformat.h"
}
#include "core/kernel/filesystem.h"
#include "core/kernel/pthread.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNgs2);

namespace {
enum WaveformInfoType {
  WAVEFORM_FILE,
  WAVEFORM_DATA,
  WAVEFORM_USER,
};

struct WaveformInfo {
  WaveformInfoType type;
  char             pad[4];

  union WaveformPtr {
    int                  fileHandle;
    SceWaveformUserFunc* userFunc;
    const void*          dataPtr;
  } ud;

  size_t size;
  long   offset;
};

static SceNgs2ChannelsCount ParseChanCount(int num) {
  LOG_USE_MODULE(libSceNgs2);

  switch (num) {
    case 1: return SceNgs2ChannelsCount::CH_1_0;

    case 2: return SceNgs2ChannelsCount::CH_2_0;

    case 6: return SceNgs2ChannelsCount::CH_5_1;

    case 8: return SceNgs2ChannelsCount::CH_7_1;
  }

  LOG_CRIT(L"Invalid channels count sent to ParseChanCount!");
  return SceNgs2ChannelsCount::INVALID;
}

static SceNgs2WaveFormType ParseWaveType(AVCodecID codec) {
  switch (codec) {
    case AVCodecID::AV_CODEC_ID_PCM_S8: return SceNgs2WaveFormType::PCM_I8;

    case AVCodecID::AV_CODEC_ID_PCM_U8: return SceNgs2WaveFormType::PCM_U8;

    case AVCodecID::AV_CODEC_ID_ATRAC9: return SceNgs2WaveFormType::ATRAC9;

    default: return SceNgs2WaveFormType::NONE;
  }
}

struct RIFFBuf {
  const uint8_t* data;
  size_t         size;
  uint64_t       offset;
};

static int readbuf(void* op, uint8_t* buf, int bufsz) {
  auto rb = (RIFFBuf*)op;
  if (rb->offset > rb->size) return AVERROR_EOF;
  int read = std::min(int(rb->size - rb->offset), bufsz);
  if (read == 0) return AVERROR_EOF;
  ::memcpy(buf, rb->data + rb->offset, read);
  return read;
}

static int64_t seekbuf(void* op, int64_t offset, int whence) {
  auto rb = (RIFFBuf*)op;
  if (whence == AVSEEK_SIZE) return rb->size;

  if (rb->data == nullptr || rb->size == 0) return -1;
  if (whence == SEEK_SET)
    rb->offset = offset;
  else if (whence == SEEK_CUR)
    rb->offset += offset;
  else if (whence == SEEK_END)
    rb->offset = rb->size - offset;

  return offset;
}

static int32_t ParseData(const uint8_t* data, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  AVFormatContext* fmtctx = avformat_alloc_context();

  auto aBufferIo = (uint8_t*)av_malloc(4096 + AV_INPUT_BUFFER_PADDING_SIZE);

  RIFFBuf rb {
      .data   = data,
      .size   = size,
      .offset = 0,
  };

  AVIOContext* avioctx = avio_alloc_context(aBufferIo, 4096, 0, &rb, &readbuf, nullptr, &seekbuf);
  fmtctx->pb           = avioctx;
  fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  int ret = avformat_open_input(&fmtctx, "nullptr", nullptr, nullptr);
  if (ret != 0) {
    LOG_ERR(L"ParseRIFF: ffmpeg failed to read passed data: %d", ret);
    return Err::Ngs2::FAIL;
  }

  AVStream* astream = nullptr;
  for (int i = 0; fmtctx->nb_streams; i++) {
    if (fmtctx->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
      astream = fmtctx->streams[i];
      break;
    }
  }

  if (astream == nullptr) {
    LOG_ERR(L"ParseRIFF: no audio stream detected!");
    return Err::Ngs2::FAIL;
  }

  wf->info.type          = ParseWaveType(astream->codecpar->codec_id);
  wf->info.sampleRate    = astream->codecpar->sample_rate;
  wf->info.channelsCount = ParseChanCount(astream->codecpar->ch_layout.nb_channels);

  // These are unknown for now
  wf->loopBeginPos = wf->loopEndPos = 0;
  wf->samplesCount                  = 0;
  wf->offset                        = 0;
  wf->size                          = 0;

  // std::vector<AVPacket>* frames = new std::vector<AVPacket>;

  // AVPacket packet;
  // while (av_read_frame(fmtctx, &packet) >= 0) {
  //   if (packet.stream_index == astream->index) {
  //     frames->insert(frames->end(), packet);
  //   }
  //   // av_packet_unref(&packet);
  // }

  wf->numBlocks         = 0;
  wf->block[0].userData = 0;
  // wf->block[0].userData = (uintptr_t)frames;

  av_free(avioctx);
  avformat_close_input(&fmtctx);

  return Ok;
}

static int32_t ProcessWaveData(WaveformInfo* wi, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);

  switch (wi->type) {
    case WAVEFORM_DATA: return ParseData((const uint8_t*)wi->ud.dataPtr, wi->size, wf);

    default: LOG_ERR(L"Unimplemented waveform reader: %d", wi->type);
  }

  /* todo: Deal with WAV/VAG files */
  return Err::Ngs2::INVALID_WAVEFORM_DATA;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNgs2";

EXPORT SYSV_ABI int32_t sceNgs2ReportRegisterHandler(uint32_t type, SceNgs2ReportHandler func, uintptr_t userData, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2ReportUnregisterHandler(SceNgs2Handle* repHan) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2CalcWaveformBlock(SceNgs2WaveformFormat* wf, uint32_t samPos, uint32_t samCount, void* out) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2CustomRackGetModuleInfo(SceNgs2Handle* rh, uint32_t modIdx, SceNgs2CustomModuleInfo* outi, size_t infosz) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformData(const void* ptr, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);

  if (ptr == nullptr) {
    return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  }

  WaveformInfo wi {
      .type = WAVEFORM_DATA,
      .ud   = {.dataPtr = ptr},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2GetWaveformFrameInfo(const SceNgs2WaveformInfo* fmt, uint32_t* outFrameSize, uint32_t* outNumFrameSamples,
                                                    uint32_t* outUnitsPerFrame, uint32_t* outNumDelaySamples) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  if (outFrameSize) *outFrameSize = 1; // Some games crashes with divide by zero exception if we set 0 here
  if (outNumFrameSamples) *outNumFrameSamples = 0;
  if (outUnitsPerFrame) *outUnitsPerFrame = 0;
  if (outNumDelaySamples) *outNumDelaySamples = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2PanGetVolumeMatrix(SceNgs2PanWork* pw, const SceNgs2PanParam* aParam, uint32_t numParams, uint32_t matrixFormat,
                                                  float* outVolumeMatrix) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2PanInit() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformFile(const char* path, long offset, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  filesystem::SceOpen flags {
      .mode = filesystem::SceOpenMode::RDONLY,
  };

  WaveformInfo wi {
      .type   = WAVEFORM_DATA,
      .ud     = {.fileHandle = filesystem::open(path, flags, {})},
      .offset = offset,
  };

  if (wi.ud.fileHandle == 0) {
    return Err::Ngs2::INVALID_WAVEFORM_DATA;
  }

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformUser(SceWaveformUserFunc* user, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  if (user == nullptr) {
    return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  }

  WaveformInfo wi {
      .type = WAVEFORM_USER,
      .ud   = {.userFunc = user},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2RackGetInfo(SceNgs2Handle* rh, SceNgs2RackInfo* outi, size_t infosz) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackSetUserData(SceNgs2Handle* rh, uintptr_t userData) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackGetUserData(SceNgs2Handle* rh, uintptr_t* userData) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *userData = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackGetVoiceHandle(SceNgs2Handle* rh, uint32_t voiceId, SceNgs2Handle** outh) {
  if (rh == nullptr) return Err::Ngs2::INVALID_RACK_HANDLE;
  if (voiceId > 0) return Err::Ngs2::INVALID_VOICE_INDEX; // todo: how much indexes??
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  // todo: write to outh voice handle from rack
  *outh = &rh->un.rack.voices[voiceId];
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackLock(SceNgs2Handle* rh) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackUnlock(SceNgs2Handle* rh) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemEnumHandles(SceNgs2Handle** outh, uint32_t maxnum) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *outh = nullptr;
  return 0;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemEnumRackHandles(SceNgs2Handle* sysh, SceNgs2Handle** outh, uint32_t maxnum) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *outh = nullptr;
  return 0;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemGetInfo(SceNgs2Handle* sysh, SceNgs2SystemInfo* outi, size_t infosz) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemSetUserData(SceNgs2Handle* sysh, uintptr_t userData) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemGetUserData(SceNgs2Handle* sysh, uintptr_t* userData) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *userData = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemLock(SceNgs2Handle* sysh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemUnlock(SceNgs2Handle* sysh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemRender(SceNgs2Handle* sysh, SceNgs2RenderBufferInfo* rbi, int32_t count) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  if (sysh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (rbi->bufferPtr == nullptr) return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  if (rbi->bufferSize == 0) return Err::Ngs2::INVALID_BUFFER_SIZE;
  if (rbi->waveType >= SceNgs2WaveFormType::MAX_TYPES) return Err::Ngs2::INVALID_WAVEFORM_TYPE;
  if (rbi->channelsCount > SceNgs2ChannelsCount::CH_7_1) return Err::Ngs2::INVALID_NUM_CHANNELS;

  for (int32_t i = 0; i < count; i++) {
    if (rbi[i].bufferPtr != nullptr) {
      std::memset(rbi[i].bufferPtr, 0, rbi[i].bufferSize);
    }
  }

  // todo: ffmpeg should convert all the wave data to `rbi->waveType`

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemSetGrainSamples(SceNgs2Handle* sysh, uint32_t samplesCount) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemSetSampleRate(SceNgs2Handle* sysh, uint32_t rate) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetMatrixInfo(SceNgs2Handle* vh, uint32_t matrixId, SceNgs2VoiceMatrixInfo* outi, size_t infosz) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetOwner(SceNgs2Handle* vh, SceNgs2Handle** outh, uint32_t* outvid) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetPortInfo(SceNgs2Handle** vh, uint32_t port, SceNgs2VoicePortInfo* outi, size_t infosz) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

static int32_t _voiceControlWaveformBlock(SceNgs2Handle* voh, const SceNgs2SamplerVoiceWaveformBlocksParam* svwfbp) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"waveblock: %d\n", svwfbp->numBlocks);
  if (svwfbp->aBlock) LOG_TRACE(L"waveptr: %llx\n", svwfbp->aBlock[0].userData);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceControl(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  switch (phead->id) {
    case SCE_NGS2_SAMPLER_VOICE_ADD_WAVEFORM_BLOCKS: return _voiceControlWaveformBlock(voh, (const SceNgs2SamplerVoiceWaveformBlocksParam*)phead);

    case (uint32_t)SceNgs2VoiceParam::SET_PORT_VOLUME: break;

    default: LOG_TRACE(L"Unhandled voice control command: (%p, %08x)", voh, phead->id);
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetState(SceNgs2Handle* voh, SceNgs2VoiceState* state, size_t size) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetStateFlags(SceNgs2Handle* voh, uint32_t* flags) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomApply() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomCalcListener() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomResetListenerParam(SceNgs2GeomListenerParam* param) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomResetSourceParam(SceNgs2GeomSourceParam* param) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreate(SceNgs2Handle* sysh, uint32_t rackId, const SceNgs2RackOption* ro, const SceNgs2ContextBufferInfo* cbi,
                                          SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreateWithAllocator(SceNgs2Handle* sysh, uint32_t rackId, const SceNgs2RackOption* ro, const SceNgs2BufferAllocator* alloc,
                                                       SceNgs2Handle** outh) {
  if (sysh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (alloc == nullptr) return Err::Ngs2::INVALID_BUFFER_ALLOCATOR;
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);

  SceNgs2ContextBufferInfo cbi = {
      .hostBuffer     = nullptr,
      .hostBufferSize = sizeof(SceNgs2Handle),
      .userData       = alloc->userData,
  };

  if (auto ret = alloc->allocHandler(&cbi)) {
    LOG_ERR(L"Ngs2Rack: Allocation failed!");
    return ret;
  }

  *outh             = (SceNgs2Handle*)cbi.hostBuffer;
  (*outh)->owner    = sysh;
  (*outh)->allocSet = true;
  (*outh)->alloc    = *alloc;
  (*outh)->cbi      = cbi;

  cbi.hostBufferSize = sizeof(SceNgs2Handle) * ((ro != nullptr) ? ro->maxVoices : 1);

  if (auto ret = alloc->allocHandler(&cbi)) {
    LOG_ERR(L"Ngs2Rack: Voice allocation failed");
    return ret;
  }

  auto vo = (*outh)->un.rack.voices = (SceNgs2Handle*)cbi.hostBuffer;
  vo->allocSet                      = true;
  vo->alloc                         = *alloc;

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackDestroy(SceNgs2Handle* rh, SceNgs2ContextBufferInfo* cbi) {
  if (rh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (rh->allocSet) {
    cbi->hostBuffer     = rh;
    cbi->hostBufferSize = sizeof(SceNgs2Handle);
    if (auto ret = rh->alloc.freeHandler(cbi)) return ret;
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackQueryBufferSize(uint32_t rackId, const SceNgs2RackOption* ro, SceNgs2ContextBufferInfo* cbi) {
  cbi->hostBufferSize = sizeof(SceNgs2Handle);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreateWithAllocator(const SceNgs2SystemOption* sysopt, SceNgs2BufferAllocator* alloc, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S(%p, %p, %p)", __FUNCTION__, sysopt, alloc, outh);
  if (alloc == nullptr || alloc->allocHandler == nullptr) return Err::Ngs2::INVALID_BUFFER_ALLOCATOR;
  if (outh == nullptr) return Err::Ngs2::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size < sizeof(SceNgs2SystemOption)) return Err::Ngs2::INVALID_OPTION_SIZE;

  SceNgs2ContextBufferInfo cbi = {
      .hostBuffer     = nullptr,
      .hostBufferSize = sizeof(SceNgs2Handle),
      .userData       = alloc->userData,
  };

  if (auto ret = alloc->allocHandler(&cbi)) {
    LOG_ERR(L"Ngs2System: Allocation failed!");
    return ret;
  }

  *outh             = (SceNgs2Handle*)cbi.hostBuffer;
  (*outh)->owner    = nullptr;
  (*outh)->allocSet = true;
  (*outh)->alloc    = *alloc;
  (*outh)->cbi      = cbi;

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreate(const SceNgs2SystemOption* sysopt, const SceNgs2ContextBufferInfo* cbi, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S(%p, %p, %p)", __FUNCTION__, sysopt, cbi, outh);
  if (outh == nullptr) return Err::Ngs2::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size < sizeof(SceNgs2SystemOption)) return Err::Ngs2::INVALID_OPTION_SIZE;
  if (cbi == nullptr || cbi->hostBuffer == nullptr || cbi->hostBufferSize < sizeof(SceNgs2Handle)) return Err::Ngs2::INVALID_BUFFER_ADDRESS;

  *outh             = (SceNgs2Handle*)cbi->hostBuffer;
  (*outh)->allocSet = false;
  (*outh)->owner    = nullptr;

  return (*outh) != nullptr ? Ok : Err::Ngs2::FAIL;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemDestroy(SceNgs2Handle* sysh, SceNgs2ContextBufferInfo* cbi) {
  if (sysh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (sysh->allocSet) {
    cbi->hostBuffer     = sysh;
    cbi->hostBufferSize = sizeof(SceNgs2Handle);
    if (auto ret = sysh->alloc.freeHandler(cbi)) return ret;
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemQueryBufferSize(const SceNgs2SystemOption* sysopt, SceNgs2ContextBufferInfo* cbi) {
  if (cbi == nullptr) return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  cbi->hostBufferSize = sizeof(SceNgs2Handle);
  return Ok;
}
}
