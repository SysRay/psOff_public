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

std::pair<AVSampleFormat, uint8_t> convFormat(SceNgs2WaveFormType type) {
  switch (type) {
    case SceNgs2WaveFormType::PCM_U8: return {AVSampleFormat::AV_SAMPLE_FMT_U8, 1};
    case SceNgs2WaveFormType::PCM_I16LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_S16, 2};
    case SceNgs2WaveFormType::PCM_I16BIG: return {AVSampleFormat::AV_SAMPLE_FMT_S16, 2};
    case SceNgs2WaveFormType::PCM_I24LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_NONE, 3};
    case SceNgs2WaveFormType::PCM_I24BIG: return {AVSampleFormat::AV_SAMPLE_FMT_NONE, 3};
    case SceNgs2WaveFormType::PCM_I32LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_S32, 4};
    case SceNgs2WaveFormType::PCM_I32BIG: return {AVSampleFormat::AV_SAMPLE_FMT_S32, 4};
    case SceNgs2WaveFormType::PCM_F32LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_FLT, 4};
    case SceNgs2WaveFormType::PCM_F32BIG: return {AVSampleFormat::AV_SAMPLE_FMT_FLT, 4};
    case SceNgs2WaveFormType::PCM_F64LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_DBL, 8};
    case SceNgs2WaveFormType::PCM_F64BIG: return {AVSampleFormat::AV_SAMPLE_FMT_DBL, 8};
    case SceNgs2WaveFormType::VAG: return {AVSampleFormat::AV_SAMPLE_FMT_FLT, 4};
    case SceNgs2WaveFormType::ATRAC9: return {AVSampleFormat::AV_SAMPLE_FMT_FLT, 4};
    default: break;
  }
  return {AVSampleFormat::AV_SAMPLE_FMT_NONE, 0};
}

std::pair<AVSampleFormat, uint8_t> convFormatPlanar(SceNgs2WaveFormType type) {
  switch (type) {
    case SceNgs2WaveFormType::PCM_U8: return {AVSampleFormat::AV_SAMPLE_FMT_U8P, 1};
    case SceNgs2WaveFormType::PCM_I16LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_S16P, 2};
    case SceNgs2WaveFormType::PCM_I16BIG: return {AVSampleFormat::AV_SAMPLE_FMT_S16P, 2};
    case SceNgs2WaveFormType::PCM_I24LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_NONE, 3};
    case SceNgs2WaveFormType::PCM_I24BIG: return {AVSampleFormat::AV_SAMPLE_FMT_NONE, 3};
    case SceNgs2WaveFormType::PCM_I32LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_S32P, 4};
    case SceNgs2WaveFormType::PCM_I32BIG: return {AVSampleFormat::AV_SAMPLE_FMT_S32P, 4};
    case SceNgs2WaveFormType::PCM_F32LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_FLTP, 4};
    case SceNgs2WaveFormType::PCM_F32BIG: return {AVSampleFormat::AV_SAMPLE_FMT_FLTP, 4};
    case SceNgs2WaveFormType::PCM_F64LITTLE: return {AVSampleFormat::AV_SAMPLE_FMT_DBLP, 8};
    case SceNgs2WaveFormType::PCM_F64BIG: return {AVSampleFormat::AV_SAMPLE_FMT_DBLP, 8};
    case SceNgs2WaveFormType::VAG: return {AVSampleFormat::AV_SAMPLE_FMT_FLTP, 4};
    case SceNgs2WaveFormType::ATRAC9: return {AVSampleFormat::AV_SAMPLE_FMT_FLTP, 4};
    default: break;
  }
  return {AVSampleFormat::AV_SAMPLE_FMT_NONE, 0};
}

constexpr bool operator==(AVChannelLayout const& rhs, AVChannelLayout const& lhs) {
  return rhs.nb_channels == lhs.nb_channels && rhs.u.mask == lhs.u.mask;
}

constexpr bool operator!=(AVChannelLayout const& rhs, AVChannelLayout const& lhs) {
  return !(rhs == lhs);
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

  AVChannelLayout curChannelLayoutIn;
  AVChannelLayout curChannelLayoutOut;
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

  auto pimpl = (PImpl*)m_pimpl;

  if (param->data == nullptr) {
    // Reset
    voice->state.bits.Empty = true;
    return true;
  }
  m_isInit = false;

  pimpl->data      = (uint8_t const*)param->data;
  pimpl->block     = param->aBlock[0];
  pimpl->curOffset = 0;

  // m_state.numDecodedSamples = 0;
  // m_state.decodedDataSize   = 0;

  if (voice->info.type != SceNgs2WaveFormType::ATRAC9 && voice->info.type != SceNgs2WaveFormType::VAG) {
    m_isCompressed = false;
    m_isInit       = true;

    voice->state.bits.Empty = false;
    return true;
  }

  return false;

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

  // pimpl->fmtctx->pb = avioctx;
  // pimpl->fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  int ret = avformat_open_input(&pimpl->fmtctx, "F:/down2/atrac9mem/snd0.at9", nullptr, nullptr);
  if (ret != 0) {
    LOG_ERR(L"Reader: ffmpeg failed to read passed data: %d", ret);
    return false;
  }

  cleanup->emplace_back([&] { avformat_close_input(&pimpl->fmtctx); });

  if (int res = avformat_find_stream_info(pimpl->fmtctx, NULL) < 0; res < 0) {
    LOG_ERR(L"avformat_find_stream_info result:%d", res);
    return false;
  }
  for (auto i = 0; i < pimpl->fmtctx->nb_streams; ++i) {
    if (pimpl->fmtctx->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
      pimpl->astream    = pimpl->fmtctx->streams[i];
      pimpl->stream_idx = i;
      break;
    }
  }

  pimpl->codec = avcodec_find_decoder(pimpl->astream->codecpar->codec_id);

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

void Reader::setNewData(void const* start, void const* end) {
  LOG_USE_MODULE(libSceNgs2);
  auto pimpl = (PImpl*)m_pimpl;

  pimpl->curOffset  = 0;
  pimpl->data       = (uint8_t const*)start;
  pimpl->block.size = (uint64_t)end - (uint64_t)start;

  voice->state.bits.Empty = false;
}

bool Reader::getAudioUncompressed(SceNgs2RenderBufferInfo* rbi, uint32_t numOutSamples, uint32_t outRate) {
  LOG_USE_MODULE(libSceNgs2);
  auto pimpl = (PImpl*)m_pimpl;

  auto const channelLayoutOut = convChannelLayout(rbi->channelsCount);
  auto const channelLayoutIn  = convChannelLayout(voice->info.channelsCount);

  if (pimpl->swrCtx == nullptr || channelLayoutOut != pimpl->curChannelLayoutOut || channelLayoutIn != pimpl->curChannelLayoutIn) {
    pimpl->curChannelLayoutOut = convChannelLayout(rbi->channelsCount);
    pimpl->curChannelLayoutIn  = convChannelLayout(voice->info.channelsCount);
    if (pimpl->curChannelLayoutOut.order == AV_CHANNEL_ORDER_UNSPEC || pimpl->curChannelLayoutIn.order == AV_CHANNEL_ORDER_UNSPEC) {
      return false;
    }

    auto const [formatOut, bytesOut] = convFormat(rbi->waveType);
    if (formatOut == AVSampleFormat::AV_SAMPLE_FMT_NONE) return false;

    auto const [formatIn, bytesIn] = convFormat(voice->info.type);
    if (formatIn == AVSampleFormat::AV_SAMPLE_FMT_NONE) return false;

    if (swr_alloc_set_opts2(&pimpl->swrCtx, &pimpl->curChannelLayoutOut, formatOut, outRate, &pimpl->curChannelLayoutIn, formatIn, voice->info.sampleRate, 0,
                            NULL)) {
      LOG_ERR(L"Reader:Couldn't alloc swr");
      return false;
    }

    swr_init(pimpl->swrCtx);
    pimpl->cleanup.emplace_back([&] { swr_free(&pimpl->swrCtx); });
  }

  // Check repeat
  if (pimpl->block.size <= pimpl->curOffset) {
    if (pimpl->block.numRepeat > 0) {
      pimpl->curOffset = 0;
      --pimpl->block.numRepeat;
    } else {
      voice->state.data = 0;

      voice->state.bits.Empty = true;
      return false;
    }
  }
  // -

  uint32_t const readSize = std::min(rbi->bufferSize, (size_t)pimpl->block.size - pimpl->curOffset);

  auto const [formatIn, bytesIn]   = convFormat(voice->info.type);
  auto const [formatOut, bytesOut] = convFormat(rbi->waveType);

  std::vector<uint8_t*> audioBuffers(((int)rbi->channelsCount));

  auto const channelSizeOut = (uint32_t)rbi->channelsCount * bytesOut;
  for (uint8_t n = 0; n < audioBuffers.size(); ++n) {
    audioBuffers[n] = &((uint8_t*)rbi->bufferPtr)[0];
  }

  if (pimpl->block.numSamples > 0) {
    std::vector<uint8_t const*> audioBuffersIn(((int)voice->info.channelsCount));

    auto const channelSizeIn = (uint32_t)voice->info.channelsCount * bytesOut;
    for (uint8_t n = 0; n < audioBuffersIn.size(); ++n) {
      audioBuffersIn[n] = &((uint8_t*)pimpl->data)[0];
    }

    auto numSamples = swr_convert(pimpl->swrCtx, (uint8_t**)&rbi->bufferPtr, numOutSamples, &pimpl->data, pimpl->block.numSamples);

    pimpl->block.numSamples = 0;
  } else {
    auto numSamples = swr_convert(pimpl->swrCtx, (uint8_t**)&rbi->bufferPtr, numOutSamples, nullptr, 0);
    if (numSamples == 0) {
      m_state.numDecodedSamples += pimpl->block.numSamples;
      m_state.decodedDataSize += pimpl->block.size;
      voice->state.bits.Empty = true;

      pimpl->curOffset = pimpl->block.size;
      return false;
    }
  }

  return true;
}

bool Reader::getAudioCompressed(SceNgs2RenderBufferInfo* rbi) {
  LOG_USE_MODULE(libSceNgs2);

  auto pimpl = (PImpl*)m_pimpl;

  // if (pimpl->swrCtx == nullptr) {
  //   auto optDstChLayout = convChannelLayout(rbi->channelsCount);
  //   if (!optDstChLayout) {
  //     return false;
  //   }
  //   auto const [format, bytesOut] = convFormat(rbi->waveType);
  //   if (format == AVSampleFormat::AV_SAMPLE_FMT_NONE) return false;

  //   if (swr_alloc_set_opts2(&pimpl->swrCtx, &(*optDstChLayout), format, voice->info.sampleRate, &pimpl->codecContext->ch_layout,
  //                           pimpl->codecContext->sample_fmt, pimpl->codecContext->sample_rate, 0, NULL)) {
  //     LOG_ERR(L"Reader:Couldn't alloc swr");
  //     return false;
  //   }

  //   swr_init(pimpl->swrCtx);
  //   pimpl->cleanup.emplace_back([&] { swr_free(&pimpl->swrCtx); });
  // }

  // size_t offset = 0;

  // while (offset < rbi->bufferSize) {
  //   // Get a new packet
  //   if (pimpl->newPacket) {
  //     pimpl->packet->dts = AV_NOPTS_VALUE;
  //     pimpl->packet->pts = AV_NOPTS_VALUE;

  //     int state = av_read_frame(pimpl->fmtctx, pimpl->packet);
  //     if (state < 0) {
  //       if (state != AVERROR_EOF) {
  //         LOG_ERR(L"av_read_frame error %d", state);
  //       } else {
  //         voice->state.bits.Empty = true;
  //       }
  //       return false;
  //     }

  //     // Skip if not a audio packet
  //     if (pimpl->packet->stream_index != pimpl->stream_idx) {
  //       continue;
  //     }

  //     pimpl->newPacket = false;
  //   }
  //   // -

  //   // Process packet
  //   if (int ret = avcodec_send_packet(pimpl->codecContext, pimpl->packet); ret < 0) {
  //     if (ret == AVERROR(EAGAIN)) {
  //       av_packet_unref(pimpl->packet);
  //       pimpl->newPacket = true;
  //       continue; // Get new frame
  //     } else if (ret == AVERROR_EOF) {
  //       voice->state.bits.Empty = true;
  //     } else {
  //       voice->state.bits.Error = true;
  //     }

  //     return false;
  //   }

  //   av_packet_unref(pimpl->packet);
  //   //- packet

  //   // Now the frames
  //   auto const retRecv = avcodec_receive_frame(pimpl->codecContext, pimpl->frame);

  //   int outNumSamples = swr_get_out_samples(pimpl->swrCtx, pimpl->frame->nb_samples);

  //   // todo get sample size, nb_channels is zero (fix)
  //   auto const [format, bytesOut] = convFormat(rbi->waveType);
  //   auto const channelSize        = (uint32_t)outNumSamples * av_get_bytes_per_sample(format);

  //   std::vector<uint8_t*> audioBuffers(((int)rbi->channelsCount));
  //   for (uint8_t n = 0; n < audioBuffers.size(); ++n) {
  //     audioBuffers[n] = &((uint8_t*)rbi->bufferPtr)[offset + n * channelSize];
  //   }

  //   for (int i = 0; i < rbi->bufferSize / 4; ++i) { // Filling the chanels with data
  //     // We should fill all the channels with available data, it seems.
  //     // The sound plays slightly quieter if we fill only two first channels.
  //     ((float*)rbi->bufferPtr)[offset + i] = ((float*)pimpl->frame->data[i % pimpl->frame->ch_layout.nb_channels])[i / (int)rbi->channelsCount];
  //   }

  //   // uint8_t*   audioBuffers[8] = {&((uint8_t*)rbi->bufferPtr)[offset], &((uint8_t*)rbi->bufferPtr)[offset],
  //   // &((uint8_t*)rbi->bufferPtr)[offset],&((uint8_t*)rbi->bufferPtr)[offset],&((uint8_t*)rbi->bufferPtr)[offset],&((uint8_t*)rbi->bufferPtr)[offset],};

  //   // if (outNumSamples = swr_convert(pimpl->swrCtx, audioBuffers.data(), outNumSamples, (uint8_t const**)pimpl->frame->extended_data,
  //   // pimpl->frame->nb_samples);
  //   //     outNumSamples < 0) {
  //   //   LOG_WARN(L"swr_convert");
  //   // }

  //   av_frame_unref(pimpl->frame);
  //   // -

  //   auto const bufferSize_ = (uint32_t)rbi->channelsCount * channelSize;

  //   // float* samples = (float*)(&((uint8_t*)rbi->bufferPtr)[offset]);
  //   // for (int i = 0; i < outNumSamples * (int)rbi->channelsCount; i++) {
  //   //   samples[i] *= 100000.0;
  //   // }

  //   offset += (uint32_t)rbi->channelsCount * channelSize;
  // }
  return true;
}

bool Reader::getAudio(SceNgs2RenderBufferInfo* rbi, uint32_t numOutSamples, uint32_t outRate) {
  if (m_isInit == false || voice->state.bits.Empty || !voice->state.bits.Playing || (voice->state.bits.Playing && voice->state.bits.Paused)) {
    return true;
  }

  if (m_isCompressed) {
    return false; // getAudioCompressed(rbi);
  } else {
    return getAudioUncompressed(rbi, numOutSamples, outRate);
  }
  return true;
}
