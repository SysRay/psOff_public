#include "reader.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include "logging.h"
#include "readFuncs.h"
#include "types.h"

#include <functional>
#include <list>

LOG_DEFINE_MODULE(libSceNgs2);

namespace {
struct PImpl {
  std::list<std::function<void(void)>> cleanup;

  userData_inerBuffer userData = {};

  int64_t numSamples = 0;

  // AVFormatContext* fmtctx     = nullptr;
  // AVStream*        astream    = nullptr;
  AVCodec const* codec      = nullptr;
  int            stream_idx = 0;

  AVCodecContext* codecContext = nullptr;

  AVFrame*  frame  = nullptr;
  AVPacket* packet = nullptr;

  // SwrContext* swrCtx = nullptr;
};
} // namespace

Reader::Reader() {
  m_pimpl = std::make_unique<PImpl>().release();
}

Reader::~Reader() {
  delete (PImpl*)m_pimpl;
}

bool Reader::init(SceNgs2SamplerVoiceWaveformBlocksParam const* param) {
  LOG_USE_MODULE(libSceNgs2);

  auto pimpl = (PImpl*)m_pimpl;

  // Init Setup
  // auto aBufferIo = (uint8_t*)av_malloc(4096 + AV_INPUT_BUFFER_PADDING_SIZE);

  // AVIOContext* avioctx = avio_alloc_context(aBufferIo, 4096, 0, &userData, readFunc_linearBuffer, nullptr, seekFunc_linearBuffer);

  // cleanup.emplace_back([&] { av_free(avioctx); });
  // cleanup.emplace_back([&] { av_free(aBufferIo); });

  // // Open the input
  // pimpl->fmtctx = avformat_alloc_context();
  // cleanup.emplace_back([&] { avformat_free_context(pimpl->fmtctx); });

  // pimpl->fmtctx->pb = avioctx;
  // pimpl->fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  // int ret = avformat_open_input(&pimpl->fmtctx, "nullptr", nullptr, nullptr);
  // if (ret != 0) {
  //   LOG_ERR(L"Reader: ffmpeg failed to read passed data: %d", ret);
  //   return false;
  // }
  // cleanup.emplace_back([&] { avformat_close_input(&pimpl->fmtctx); });

  // pimpl->stream_idx = av_find_best_stream(pimpl->fmtctx, AVMEDIA_TYPE_AUDIO, -1, -1, &pimpl->codec, 0);

  // if (pimpl->stream_idx < 0) {
  //   LOG_ERR(L"Reader: no audio stream");
  //   return false;
  // }

  // pimpl->astream = pimpl->fmtctx->streams[pimpl->stream_idx];

  // pimpl->codecContext = avcodec_alloc_context3(pimpl->codec);
  // if (auto err = avcodec_parameters_to_context(pimpl->codecContext, pimpl->astream->codecpar); err != 0) {
  //   LOG_ERR(L"Reader: avcodec_parameters_to_context err:%d", err);
  //   return false;
  // }
  // cleanup.emplace_back([&] { avcodec_free_context(&pimpl->codecContext); });

  // // Setup multithreading
  // if (pimpl->codec->capabilities | AV_CODEC_CAP_FRAME_THREADS)
  //   pimpl->codecContext->thread_type = FF_THREAD_FRAME;
  // else if (pimpl->codec->capabilities | AV_CODEC_CAP_SLICE_THREADS)
  //   pimpl->codecContext->thread_type = FF_THREAD_SLICE;
  // else
  //   pimpl->codecContext->thread_count = 1; // don't use multithreading
  // // -

  // if (auto err = avcodec_open2(pimpl->codecContext, pimpl->codec, NULL); err < 0) {
  //   LOG_ERR(L"Reader: avcodec_open2 err:%d", err);
  //   return false;
  // }

  // if (swr_alloc_set_opts2(&pimpl->swrCtx, &pimpl->codecContext->ch_layout, AVSampleFormat::AV_SAMPLE_FMT_S16, pimpl->codecContext->sample_rate,
  //                         &pimpl->codecContext->ch_layout, pimpl->codecContext->sample_fmt, pimpl->codecContext->sample_rate, 0, NULL)) {
  //   LOG_ERR(L"Reader:Couldn't alloc swr");
  //   return false;
  // }

  // swr_init(pimpl->swrCtx);
  // cleanup.emplace_back([&] { swr_free(&pimpl->swrCtx); });

  // pimpl->frame = av_frame_alloc();
  // cleanup.emplace_back([&] { av_frame_free(&pimpl->frame); });

  // pimpl->packet = av_packet_alloc();
  // cleanup.emplace_back([&] { av_packet_free(&pimpl->packet); });

  m_isInit = true;

  return true;
}

bool Reader::getAudio(SceNgs2Handle_voice* handle, void* buffer, size_t bufferSize) {
  if (m_isInit == false || !handle->state.bits.Playing || (handle->state.bits.Playing && handle->state.bits.Paused)) {
    std::memset(buffer, 0, bufferSize);
    return true;
  }
  LOG_USE_MODULE(libSceNgs2);

  auto pimpl = (PImpl*)m_pimpl;

  std::memset(buffer, 0, bufferSize);

  // size_t offset = 0;

  // int const  outNumSamples = swr_get_out_samples(pimpl->swrCtx, pimpl->frame->nb_samples);
  // auto const bufferSize_   = pimpl->frame->ch_layout.nb_channels * outNumSamples * av_get_bytes_per_sample((AVSampleFormat)pimpl->frame->format);

  // while (offset < bufferSize) {
  //   // Get a new packet
  //   if (pimpl->packet == nullptr) {
  //     int state = av_read_frame(pimpl->fmtctx, pimpl->packet);
  //     if (state < 0) {
  //       if (state != AVERROR_EOF) {
  //         LOG_ERR(L"av_read_frame error %d", state);
  //       } else {
  //         handle->state.bits.Empty   = true;
  //         handle->state.bits.Playing = false;
  //       }
  //       std::memset(buffer, 0, bufferSize);
  //       return false;
  //     }
  //   }
  //   // -

  //   // Process packet
  //   if (int ret = avcodec_send_packet(pimpl->codecContext, pimpl->packet); ret < 0) {
  //     if (ret == AVERROR(EAGAIN)) {
  //       av_packet_unref(pimpl->packet);
  //       pimpl->packet = nullptr;
  //       continue; // Get new frame
  //     } else if (ret == AVERROR_EOF) {
  //       handle->state.bits.Empty = true;
  //     } else {
  //       handle->state.bits.Error = true;
  //     }
  //     handle->state.bits.Playing = false;
  //     std::memset(buffer, 0, bufferSize);
  //     return false;
  //   }

  //   av_packet_unref(pimpl->packet);
  //   //- packet

  //   // Now the frames
  //   auto const retRecv = avcodec_receive_frame(pimpl->codecContext, pimpl->frame);

  //   uint8_t* audioBuffers[1] = {(uint8_t*)buffer + offset};
  //   if (swr_convert(pimpl->swrCtx, audioBuffers, outNumSamples, (uint8_t const**)pimpl->frame->extended_data, pimpl->frame->nb_samples) < 0) {
  //     LOG_WARN(L"swr_convert");
  //   }

  //   av_frame_unref(pimpl->frame);
  //   // -

  //   offset += bufferSize_;
  // }
  return true;
}