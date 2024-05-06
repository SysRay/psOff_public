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

  AVStream*      astream    = nullptr;
  AVCodec const* codec      = nullptr;
  int            stream_idx = 0;

  AVCodecContext* codecContext = nullptr;

  AVFrame*    frame  = nullptr;
  SwrContext* swrCtx = nullptr;
};
} // namespace

Reader::Reader() {
  m_pimpl = std::make_unique<PImpl>().release();
}

Reader::~Reader() {
  delete (PImpl*)m_pimpl;
}

bool Reader::init(void const* data, size_t size) {
  LOG_USE_MODULE(libSceNgs2);

  auto pimpl = (PImpl*)m_pimpl;

  auto& cleanup  = pimpl->cleanup;
  auto& userData = pimpl->userData;

  userData.ptr       = data;
  userData.size      = size;
  userData.curOffset = 0;

  // Init Setup
  auto aBufferIo = (uint8_t*)av_malloc(4096 + AV_INPUT_BUFFER_PADDING_SIZE);

  AVIOContext* avioctx = avio_alloc_context(aBufferIo, 4096, 0, &userData, readFunc_linearBuffer, nullptr, seekFunc_linearBuffer);

  cleanup.emplace_back([&] { av_free(avioctx); });
  cleanup.emplace_back([&] { av_free(aBufferIo); });

  // Open the input
  AVFormatContext* fmtctx = avformat_alloc_context();
  cleanup.emplace_back([&] { avformat_free_context(fmtctx); });

  fmtctx->pb = avioctx;
  fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  int ret = avformat_open_input(&fmtctx, "nullptr", nullptr, nullptr);
  if (ret != 0) {
    LOG_ERR(L"Reader: ffmpeg failed to read passed data: %d", ret);
    return false;
  }
  cleanup.emplace_back([&] { avformat_close_input(&fmtctx); });

  pimpl->stream_idx = av_find_best_stream(fmtctx, AVMEDIA_TYPE_AUDIO, -1, -1, &pimpl->codec, 0);

  if (pimpl->stream_idx < 0) {
    LOG_ERR(L"Reader: no audio stream");
    return false;
  }

  pimpl->astream = fmtctx->streams[pimpl->stream_idx];

  pimpl->codecContext = avcodec_alloc_context3(pimpl->codec);
  if (auto err = avcodec_parameters_to_context(pimpl->codecContext, pimpl->astream->codecpar); err != 0) {
    LOG_ERR(L"Reader: avcodec_parameters_to_context err:%d", err);
    return false;
  }
  cleanup.emplace_back([&] { avcodec_free_context(&pimpl->codecContext); });

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

  if (swr_alloc_set_opts2(&pimpl->swrCtx, &pimpl->codecContext->ch_layout, AVSampleFormat::AV_SAMPLE_FMT_S16, pimpl->codecContext->sample_rate,
                          &pimpl->codecContext->ch_layout, pimpl->codecContext->sample_fmt, pimpl->codecContext->sample_rate, 0, NULL)) {
    LOG_ERR(L"Reader:Couldn't alloc swr");
    return false;
  }

  swr_init(pimpl->swrCtx);
  cleanup.emplace_back([&] { swr_free(&pimpl->swrCtx); });

  pimpl->frame = av_frame_alloc();
  cleanup.emplace_back([&] { av_frame_free(&pimpl->frame); });

  m_isInit = true;

  return true;
}

bool Reader::getAudio(void* buffer, size_t bufferSize) {
  std::memset(buffer, 0, bufferSize);
  return true;
}