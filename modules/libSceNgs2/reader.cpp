#include "reader.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include "logging.h"
#include "readFuncs.h"
#include "riffTypes.h"
#include "types.h"

#include <functional>
#include <list>

LOG_DEFINE_MODULE(libSceNgs2);

namespace {

AVSampleFormat convFormat(SceNgs2WaveFormType type) {
  switch (type) {
    case SceNgs2WaveFormType::PCM_U8: return AVSampleFormat::AV_SAMPLE_FMT_U8;
    case SceNgs2WaveFormType::PCM_I16LITTLE: return AVSampleFormat::AV_SAMPLE_FMT_S16;
    case SceNgs2WaveFormType::PCM_I16BIG: return AVSampleFormat::AV_SAMPLE_FMT_S16;
    case SceNgs2WaveFormType::PCM_I24LITTLE: return AVSampleFormat::AV_SAMPLE_FMT_NONE;
    case SceNgs2WaveFormType::PCM_I24BIG: return AVSampleFormat::AV_SAMPLE_FMT_NONE;
    case SceNgs2WaveFormType::PCM_I32LITTLE: return AVSampleFormat::AV_SAMPLE_FMT_S32;
    case SceNgs2WaveFormType::PCM_I32BIG: return AVSampleFormat::AV_SAMPLE_FMT_S32;
    case SceNgs2WaveFormType::PCM_F32LITTLE: return AVSampleFormat::AV_SAMPLE_FMT_FLT;
    case SceNgs2WaveFormType::PCM_F32BIG: return AVSampleFormat::AV_SAMPLE_FMT_FLT;
    case SceNgs2WaveFormType::PCM_F64LITTLE: return AVSampleFormat::AV_SAMPLE_FMT_DBL;
    case SceNgs2WaveFormType::PCM_F64BIG: return AVSampleFormat::AV_SAMPLE_FMT_DBL;
    case SceNgs2WaveFormType::VAG: return AVSampleFormat::AV_SAMPLE_FMT_FLT;    // todo
    case SceNgs2WaveFormType::ATRAC9: return AVSampleFormat::AV_SAMPLE_FMT_FLT; // todo
    default: break;
  }
  return AVSampleFormat::AV_SAMPLE_FMT_NONE;
}

struct PImpl {
  std::list<std::function<void(void)>> cleanup;

  uint8_t const*       data = nullptr;
  SceNgs2WaveformBlock block;

  uint32_t curOffset = 0;

  userData_inerBuffer userData   = {};
  AVFormatContext*    fmtctx     = nullptr;
  AVStream*           astream    = nullptr;
  AVCodec const*      codec      = nullptr;
  int                 stream_idx = 0;

  AVCodecContext* codecContext = nullptr;

  AVFrame*  frame     = nullptr;
  AVPacket* packet    = nullptr;
  bool      newPacket = true;

  SwrContext* swrCtx = nullptr;
};
} // namespace

Reader::Reader(SceNgs2Handle_voice* voice): voice(voice) {
  m_pimpl = std::make_unique<PImpl>().release();
}

Reader::~Reader() {
  delete (PImpl*)m_pimpl;
}

bool Reader::init(SceNgs2SamplerVoiceWaveformBlocksParam const* param) {
  LOG_USE_MODULE(libSceNgs2);
  if (m_isInit) return true; // todo

  auto pimpl = (PImpl*)m_pimpl;

  if (param->data == nullptr) {
    // Reset
    voice->state.bits.Empty = true;

    delete pimpl;
    m_pimpl = std::make_unique<PImpl>().release();
    return true;
  }
  m_isInit = false;

  pimpl->data      = (uint8_t const*)param->data;
  pimpl->block     = param->aBlock[0];
  pimpl->curOffset = 0;

  // Check if riff or uncompressed
  auto riffHeader = (RiffWaveHeader const*)param->data;
  if (memcmp(riffHeader->chunkID, "RIFF", 4) != 0 || memcmp(riffHeader->riffType, "WAVE", 4) != 0) {
    m_isCompressed = false;
    return true;
  }

  // parse riff
  m_isCompressed = true;

  auto cleanup  = &pimpl->cleanup;
  auto userData = &pimpl->userData;

  userData->ptr       = param->data;
  userData->size      = param->aBlock[0].offset + param->aBlock[0].size;
  userData->curOffset = 0;

  pimpl->newPacket = true;

  // Init Setup
  auto aBufferIo = (uint8_t*)av_malloc(4096 + AV_INPUT_BUFFER_PADDING_SIZE);

  AVIOContext* avioctx = avio_alloc_context(aBufferIo, 4096, 0, userData, readFunc_linearBuffer, nullptr, seekFunc_linearBuffer);

  cleanup->emplace_back([&] { av_free(avioctx); });
  cleanup->emplace_back([&] { av_free(aBufferIo); });

  // Open the input
  pimpl->fmtctx = avformat_alloc_context();

  pimpl->fmtctx->pb = avioctx;
  pimpl->fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  int ret = avformat_open_input(&pimpl->fmtctx, nullptr, nullptr, nullptr);
  if (ret != 0) {
    LOG_ERR(L"Reader: ffmpeg failed to read passed data: %d", ret);
    return false;
  }

  cleanup->emplace_back([&] { avformat_close_input(&pimpl->fmtctx); });

  if (int res = avformat_find_stream_info(pimpl->fmtctx, NULL) < 0; res < 0) {
    LOG_ERR(L"avformat_find_stream_info result:%d", res);
    return false;
  }

  pimpl->stream_idx = av_find_best_stream(pimpl->fmtctx, AVMEDIA_TYPE_AUDIO, -1, -1, &pimpl->codec, 0);
  pimpl->astream    = pimpl->fmtctx->streams[pimpl->stream_idx];

  pimpl->codecContext = avcodec_alloc_context3(pimpl->codec);
  if (auto err = avcodec_parameters_to_context(pimpl->codecContext, pimpl->astream->codecpar); err != 0) {
    LOG_ERR(L"Reader: avcodec_parameters_to_context err:%d", err);
    return false;
  }
  cleanup->emplace_back([&] { avcodec_free_context(&pimpl->codecContext); });

  // Setup multithreading
  if (pimpl->codec->capabilities | AV_CODEC_CAP_FRAME_THREADS)
    pimpl->codecContext->thread_type = FF_THREAD_FRAME;
  else if (pimpl->codec->capabilities | AV_CODEC_CAP_SLICE_THREADS)
    pimpl->codecContext->thread_type = FF_THREAD_SLICE;
  else
    pimpl->codecContext->thread_count = 1; // don't use multithreading
  // -

  if (auto err = avcodec_open2(pimpl->codecContext, pimpl->codec, NULL); err < 0) {
    LOG_ERR(L"Reader: avcodec_open2 err:%d", err);
    return false;
  }

  pimpl->frame = av_frame_alloc();
  cleanup->emplace_back([&] { av_frame_free(&pimpl->frame); });

  pimpl->packet = av_packet_alloc();
  cleanup->emplace_back([&] { av_packet_free(&pimpl->packet); });

  m_isInit                = true;
  voice->state.bits.Empty = false;
  return true;
}

bool Reader::getAudioUncompressed(SceNgs2RenderBufferInfo* rbi) {
  auto pimpl = (PImpl*)m_pimpl;

  // Check repeat
  if (pimpl->block.size <= pimpl->curOffset) {
    if (pimpl->block.numRepeat > 0) {
      pimpl->curOffset = 0;
      --pimpl->block.numRepeat;
    } else {
      voice->state.bits.Empty = true;
      return false;
    }
  }
  // -

  uint32_t const readSize = std::min(rbi->bufferSize, (size_t)pimpl->block.size - pimpl->curOffset);

  std::memcpy(rbi->bufferPtr, pimpl->data + pimpl->curOffset, readSize);
  pimpl->curOffset += readSize;

  return true;
}

bool Reader::getAudioCompressed(SceNgs2RenderBufferInfo* rbi) {
  LOG_USE_MODULE(libSceNgs2);

  auto pimpl = (PImpl*)m_pimpl;

  if (pimpl->swrCtx == nullptr) {
    auto optDstChLayout = convChannelLayout(rbi->channelsCount);
    if (!optDstChLayout) {
      return false;
    }
    auto format = convFormat(rbi->waveType);
    if (format == AVSampleFormat::AV_SAMPLE_FMT_NONE) return false;

    if (swr_alloc_set_opts2(&pimpl->swrCtx, &(*optDstChLayout), format, voice->info.sampleRate, &pimpl->codecContext->ch_layout,
                            pimpl->codecContext->sample_fmt, pimpl->codecContext->sample_rate, 0, NULL)) {
      LOG_ERR(L"Reader:Couldn't alloc swr");
      return false;
    }

    swr_init(pimpl->swrCtx);
    pimpl->cleanup.emplace_back([&] { swr_free(&pimpl->swrCtx); });
  }

  size_t offset = 0;

  while (offset < rbi->bufferSize) {
    // Get a new packet
    if (pimpl->newPacket) {
      pimpl->packet->dts = AV_NOPTS_VALUE;
      pimpl->packet->pts = AV_NOPTS_VALUE;

      int state = av_read_frame(pimpl->fmtctx, pimpl->packet);
      if (state < 0) {
        if (state != AVERROR_EOF) {
          LOG_ERR(L"av_read_frame error %d", state);
        } else {
          voice->state.bits.Empty   = true;
          voice->state.bits.Playing = false;
        }
        return false;
      }

      // Skip if not a audio packet
      if (pimpl->packet->stream_index != pimpl->stream_idx) {
        continue;
      }

      pimpl->newPacket = false;
    }
    // -

    // Process packet
    if (int ret = avcodec_send_packet(pimpl->codecContext, pimpl->packet); ret < 0) {
      if (ret == AVERROR(EAGAIN)) {
        av_packet_unref(pimpl->packet);
        pimpl->newPacket = true;
        continue; // Get new frame
      } else if (ret == AVERROR_EOF) {
        voice->state.bits.Empty = true;
      } else {
        voice->state.bits.Error = true;
      }
      voice->state.bits.Playing = false;

      return false;
    }

    av_packet_unref(pimpl->packet);
    //- packet

    // Now the frames
    auto const retRecv = avcodec_receive_frame(pimpl->codecContext, pimpl->frame);

    int outNumSamples = swr_get_out_samples(pimpl->swrCtx, pimpl->frame->nb_samples);

    // todo get sample size, nb_channels is zero (fix)
    uint8_t* audioBuffers[1] = {&((uint8_t*)rbi->bufferPtr)[offset]};

    if (outNumSamples = swr_convert(pimpl->swrCtx, audioBuffers, outNumSamples, (uint8_t const**)pimpl->frame->data, pimpl->frame->nb_samples);
        outNumSamples < 0) {
      LOG_WARN(L"swr_convert");
    }

    av_frame_unref(pimpl->frame);
    // -

    auto const bufferSize_ = (uint32_t)rbi->channelsCount * outNumSamples * av_get_bytes_per_sample(convFormat(rbi->waveType));

    // float* samples = (float*)(&((uint8_t*)rbi->bufferPtr)[offset]);
    // for (int i = 0; i < outNumSamples * (int)rbi->channelsCount; i++) {
    //   samples[i] *= 100000.0;
    // }

    offset += bufferSize_;
  }
  return true;
}

bool Reader::getAudio(SceNgs2RenderBufferInfo* rbi) {
  if (m_isInit == false || !voice->state.bits.Playing || (voice->state.bits.Playing && voice->state.bits.Paused)) {
    return true;
  }

  if (m_isCompressed) {
    return getAudioCompressed(rbi);
  } else {
    return getAudioUncompressed(rbi);
  }
  return true;
}