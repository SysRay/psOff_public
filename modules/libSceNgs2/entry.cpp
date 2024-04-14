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

static int32_t ParseRIFF(const uint8_t* data, size_t size, SceNgs2WaveformFormat* wf) {
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
    return Err::FAIL;
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
    return Err::FAIL;
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

  // wf->numBlocks         = 1;
  // wf->block[0].userData = (uintptr_t)frames;

  av_free(avioctx);
  avformat_close_input(&fmtctx);

  return Ok;
}

static int32_t ParseVAG(const uint8_t* data, size_t size, SceNgs2WaveformFormat* wf) {
  return Err::INVALID_WAVEFORM_DATA;
}

static int32_t ProcessWaveData(WaveformInfo* wi, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);

  switch (wi->type) {
    case WAVEFORM_DATA:
      switch (auto ftype = *(uint32_t*)wi->ud.dataPtr) {
        case 0x46464952: // RIFF audio
          return ParseRIFF((const uint8_t*)wi->ud.dataPtr, wi->size, wf);

        case 0x70474156: // VAG audio
          return ParseVAG((const uint8_t*)wi->ud.dataPtr, wi->size, wf);

        default: LOG_ERR(L"Unimplemented filetype: %08x", ftype);
      }
      break;

    default: LOG_ERR(L"Unimplemented waveform reader: %d", wi->type);
  }

  /* todo: Deal with WAV/VAG files */
  return Err::INVALID_WAVEFORM_DATA;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNgs2";

EXPORT SYSV_ABI int32_t sceNgs2CalcWaveformBlock(SceNgs2WaveformFormat* wf, uint32_t samPos, uint32_t samCount, void* out) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformData(const void* ptr, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);

  if (ptr == nullptr) {
    return Err::INVALID_BUFFER_ADDRESS;
  }

  WaveformInfo wi {
      .type = WAVEFORM_DATA,
      .ud   = {.dataPtr = ptr},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
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
    return Err::INVALID_WAVEFORM_DATA;
  }

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformUser(SceWaveformUserFunc* user, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  if (user == nullptr) {
    return Err::INVALID_BUFFER_ADDRESS;
  }

  WaveformInfo wi {
      .type = WAVEFORM_USER,
      .ud   = {.userFunc = user},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreateWithAllocator(SceNgs2Handle* sysh, uint32_t rackId, const SceNgs2RackOption* ro, const SceNgs2BufferAllocator* alloc,
                                                       SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);

  if ((*outh = new SceNgs2Handle() /*todo: use passed allocator?*/) != nullptr) {
    (*outh)->alloc = *alloc;
    (*outh)->owner = sysh;
  }

  return (*outh) != nullptr ? Ok : Err::FAIL;
}

EXPORT SYSV_ABI int32_t sceNgs2RackDestroy(SceNgs2Handle* rh, void*) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackGetVoiceHandle(SceNgs2Handle* rh, uint32_t voiceId, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  // todo: write to outh voice handle from rack
  *outh = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackLock(SceNgs2Handle* rh) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackUnlock(SceNgs2Handle* rh) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreateWithAllocator(SceNgs2SystemOption* sysopt, SceNgs2BufferAllocator* alloc, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S(%p, %p, %p)", __FUNCTION__, sysopt, alloc, outh);
  if (alloc == nullptr || alloc->allocHandler == nullptr) return Err::INVALID_BUFFER_ALLOCATOR;
  if (outh == nullptr) return Err::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size != sizeof(SceNgs2SystemOption)) return Err::INVALID_OPTION_SIZE;
  // todo: dealloc if (*outh) != nullptr

  if ((*outh = new SceNgs2Handle /*todo: use passed allocator?*/) != nullptr) {
    (*outh)->alloc = *alloc;
    (*outh)->owner = nullptr;
  }

  return (*outh) != nullptr ? Ok : Err::FAIL;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemDestroy(SceNgs2Handle* sysh, SceNgs2ContextBufferInfo* cbi) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemRender(SceNgs2Handle* sysh, SceNgs2RenderBufferInfo* rbi, int32_t count) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  if (sysh == nullptr) return Err::INVALID_SYSTEM_HANDLE;
  if (rbi->bufferPtr == nullptr) return Err::INVALID_BUFFER_ADDRESS;
  if (rbi->bufferSize == 0) return Err::INVALID_BUFFER_SIZE;
  if (rbi->waveType >= SceNgs2WaveFormType::MAX_TYPES) return Err::INVALID_WAVEFORM_TYPE;
  if (rbi->channelsCount > SceNgs2ChannelsCount::CH_7_1) return Err::INVALID_NUM_CHANNELS;

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

EXPORT SYSV_ABI int32_t sceNgs2VoiceControl(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  switch (phead->id) {
    case SCE_NGS2_SAMPLER_VOICE_ADD_WAVEFORM_BLOCKS: {
      auto param = (SceNgs2SamplerVoiceWaveformBlocksParam*)phead;
      // todo: save wavedata somewhere in voice handle
    } break;

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

EXPORT SYSV_ABI int32_t sceNgs2RackQueryBufferSize(uint32_t rackId, const SceNgs2RackOption* ro, SceNgs2ContextBufferInfo* cbi) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  cbi->hostBufferSize = 100;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreate(SceNgs2SystemOption* sysopt, SceNgs2ContextBufferInfo* cbi, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  if (outh == nullptr) return Err::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size != sizeof(SceNgs2SystemOption)) return Err::INVALID_OPTION_SIZE;

  *outh = new SceNgs2Handle;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemQueryBufferSize(const SceNgs2SystemOption* sysopt, SceNgs2ContextBufferInfo* cbi) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  cbi->hostBufferSize = 100;
  return Ok;
}
}
