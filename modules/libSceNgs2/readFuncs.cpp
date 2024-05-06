#include "readFuncs.h"

#include "core/fileManager/ifile.h"
#include "core/kernel/filesystem.h"
#include "logging.h"
extern "C" {
#include <libavformat/avformat.h>
}
#include <functional>
#include <list>
LOG_DEFINE_MODULE(libSceNgs2);

namespace {
static SceNgs2ChannelsCount convChanCount(int num) {
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

static SceNgs2WaveFormType convWaveType(AVCodecID codec) {
  switch (codec) {
    case AVCodecID::AV_CODEC_ID_PCM_S8: return SceNgs2WaveFormType::PCM_I8;

    case AVCodecID::AV_CODEC_ID_PCM_U8: return SceNgs2WaveFormType::PCM_U8;

    case AVCodecID::AV_CODEC_ID_ATRAC9: return SceNgs2WaveFormType::ATRAC9;

    default: return SceNgs2WaveFormType::NONE;
  }
}
} // namespace

int readFunc_linearBuffer(void* userData_, uint8_t* buf, int size) {
  auto userData = (userData_inerBuffer*)userData_;

  int const dataLeft = userData->size - userData->curOffset;
  auto      readSize = std::min(dataLeft, size);

  if (readSize > 0) {
    userData->curOffset += readSize;
    ::memcpy(buf, userData->ptr, readSize);
  }

  return readSize;
}

int64_t seekFunc_linearBuffer(void* userData_, int64_t offset, int whence) {
  auto userData = (userData_inerBuffer*)userData_;

  switch ((SceWhence)whence) {
    case SceWhence::beg: userData->curOffset = offset; break;
    case SceWhence::cur:
      if (userData->curOffset < offset)
        userData->curOffset = 0;
      else
        userData->curOffset += offset;
      break;
    case SceWhence::end: userData->curOffset = std::min(userData->size, userData->size + offset); break;
  }

  return userData->curOffset;
}

int readFunc_file(void* userData_, uint8_t* buf, int size) {
  auto handle = (int&)userData_;
  return filesystem::read(handle, buf, size);
}

int64_t seekFunc_file(void* userData_, int64_t offset, int whence) {
  auto handle = (int&)userData_;
  return filesystem::lseek(handle, offset, whence);
}

int readFunc_user(void* userData_, uint8_t* buf, int size) {
  auto userData = (userData_user*)userData_;

  auto read = userData->func(userData->userData, userData->curOffset, buf, size);
  userData->curOffset += read;
  return read;
}

int64_t seekFunc_user(void* userData_, int64_t offset, int whence) {
  auto userData = (userData_user*)userData_;

  return 0; // undefined
}

int32_t parseWave(funcReadBuf_t readFunc, funcSeekBuf_t seekFunc, void* userData, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);

  std::list<std::function<void(void)>> cleanup;

  // Setup
  auto aBufferIo = (uint8_t*)av_malloc(4096 + AV_INPUT_BUFFER_PADDING_SIZE);

  AVIOContext* avioctx = avio_alloc_context(aBufferIo, 4096, 0, userData, readFunc, nullptr, seekFunc);

  cleanup.emplace_back([&] { av_free(avioctx); });
  cleanup.emplace_back([&] { av_free(aBufferIo); });

  // Open the input
  AVFormatContext* fmtctx = avformat_alloc_context();
  cleanup.emplace_back([&] { avformat_free_context(fmtctx); });

  fmtctx->pb = avioctx;
  fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  int ret = avformat_open_input(&fmtctx, "nullptr", nullptr, nullptr);
  if (ret != 0) {
    LOG_ERR(L"ParseRIFF: ffmpeg failed to read passed data: %d", ret);
    return Err::Ngs2::FAIL;
  }
  cleanup.emplace_back([&] { avformat_close_input(&fmtctx); });

  AVStream*      astream = nullptr;
  AVCodec const* codec   = nullptr;

  {
    auto stream_idx = av_find_best_stream(fmtctx, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);

    if (stream_idx < 0) {
      LOG_ERR(L"ParseRIFF: no audio stream");
      return Err::Ngs2::FAIL;
    }

    astream = fmtctx->streams[stream_idx];
  }

  // Fill data
  wf->info.type          = convWaveType(astream->codecpar->codec_id);
  wf->info.sampleRate    = astream->codecpar->sample_rate;
  wf->info.channelsCount = convChanCount(astream->codecpar->ch_layout.nb_channels);

  // These are unknown for now
  wf->loopBeginPos = wf->loopEndPos = 0;

  wf->samplesCount = astream->nb_frames;

  auto const totalSize = seekFunc(userData, 0, (int)SceWhence::end);

  wf->offset = 0;
  wf->size   = totalSize;

  auto& block   = wf->block[0];
  block.offset  = 0;
  block.size    = totalSize;
  wf->numBlocks = 1;
  return Ok;
}