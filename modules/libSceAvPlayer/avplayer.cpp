#include "avplayer.h"

#include "typesEx.h"

#include <fileManager.h>
#include <gpuMemoryManagerExports.h>
#include <logging.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <queue>

#include <condition_variable>
#include <mutex>

LOG_DEFINE_MODULE(AvPlayer);

namespace {

bool openCodecContext(int* stream_idx, AVFormatContext* fmt_ctx, enum AVMediaType type, AVCodecContext** outCodecContext) {
  LOG_USE_MODULE(AvPlayer);

  AVCodec const* codec;
  *stream_idx = av_find_best_stream(fmt_ctx, type, -1, -1, &codec, 0);
  if (*stream_idx < 0) {
    LOG_ERR(L"openCodecContext: no %S stream", av_get_media_type_string(type));
    return false;
  }
  LOG_DEBUG(L"using codec: %S [%S]", codec->name, codec->long_name);

  AVCodecParameters* codecParams = fmt_ctx->streams[*stream_idx]->codecpar;

  *outCodecContext = avcodec_alloc_context3(codec);
  if (avcodec_parameters_to_context(*outCodecContext, codecParams) != 0) {
    LOG_ERR(L"avcodec_parameters_to_context %S", av_get_media_type_string(type));
    return false;
  }

  // Setup multithreading
  if (codec->capabilities | AV_CODEC_CAP_FRAME_THREADS)
    (*outCodecContext)->thread_type = FF_THREAD_FRAME;
  else if (codec->capabilities | AV_CODEC_CAP_SLICE_THREADS)
    (*outCodecContext)->thread_type = FF_THREAD_SLICE;
  else
    (*outCodecContext)->thread_count = 1; // don't use multithreading
  // -

  if (avcodec_open2(*outCodecContext, codec, NULL) < 0) {
    LOG_ERR(L"avcodec_open2: %S codec", av_get_media_type_string(type));
    avcodec_free_context(outCodecContext);
    return false;
  }

  return true;
}
} // namespace

class Avplayer: public IAvplayer {
  std::mutex m_mutex_int;

  SceAvPlayerMemAllocator m_memAlloc;
  std::filesystem::path   m_filename;

  bool m_isLoop        = false;
  bool m_isVideoActive = false, m_isAudioActive = false;

  void*                m_videoBuffer = nullptr;
  std::vector<uint8_t> m_audioBuffer;

  std::array<int, 4>   m_videoStride;
  std::array<void*, 4> m_videoPlane;

  int64_t m_startTime = 0;

  // Avplayer Data
  AVFormatContext* m_pFmtCtx = nullptr;
  SwsContext*      m_swsCtx  = nullptr;
  SwrContext*      m_swrCtx  = nullptr;

  // Decode Thread Vars
  std::mutex m_mutexDecode;

  bool m_isStop = false;

  std::unique_ptr<std::thread> m_decodeThread;
  std::condition_variable      m_condDecode; // Notify Decoding Thread
  // - Decode Thread

  struct DataBase {
    int             streamIndex  = -1;
    AVCodecContext* codecContext = nullptr;
    AVStream*       stream       = nullptr;
    AVFrame*        frame        = nullptr;

    bool getNewFrame = true;

    bool                    pending = false;
    std::queue<AVPacket*>   decodeQueue;
    std::condition_variable m_cond; // Notify Client for new data

    void destroy() {
      if (codecContext != nullptr) avcodec_free_context(&codecContext);
      if (frame != nullptr) av_frame_free(&frame);
    }
  } m_video, m_audio;

  void destroyAVData() {
    if (m_pFmtCtx != nullptr) {
      avformat_close_input(&m_pFmtCtx);
    }
    if (m_swsCtx != nullptr) {
      sws_freeContext(m_swsCtx);
    }
    if (m_swrCtx != nullptr) {
      swr_free(&m_swrCtx);
    }

    m_video.destroy();
    m_audio.destroy();

    m_isStop = true;
    m_condDecode.notify_one();
    if (m_decodeThread) m_decodeThread->join();
  }

  std::unique_ptr<std::thread> threadFunc();

  public:
  Avplayer(SceAvPlayerMemAllocator const& initData): m_memAlloc(initData) {}

  virtual ~Avplayer() {
    LOG_USE_MODULE(AvPlayer);
    LOG_DEBUG(L"destruct");
    destroyAVData();
  };

  bool setFile(const char* filename) final;
  void setLoop(bool isLoop) final {
    std::unique_lock const lock(m_mutex_int);
    m_isLoop = isLoop;
  }

  bool getVideoData(void* videoInfo, bool isEx) final;
  bool getAudioData(SceAvPlayerFrameInfo* videoInfo) final;

  bool isPlaying() final {
    LOG_USE_MODULE(AvPlayer);
    std::unique_lock lock(m_mutexDecode);

    LOG_TRACE(L"isPlaying:%u", m_isVideoActive | m_isAudioActive);
    return m_isVideoActive | m_isAudioActive;
  }

  void stop() final {
    LOG_USE_MODULE(AvPlayer);
    LOG_DEBUG(L"stop");

    m_isStop = true;
    m_video.m_cond.notify_one();
    m_audio.m_cond.notify_one();

    if (m_videoBuffer != nullptr) m_memAlloc.deallocateTexture(m_memAlloc.objectPointer, m_videoBuffer);
    m_videoBuffer = nullptr;
  }
};

std::unique_ptr<IAvplayer> createAvPlayer(SceAvPlayerMemAllocator const& initData) {
  auto inst = std::make_unique<Avplayer>(initData);

  return inst;
}

bool Avplayer::setFile(const char* filename) {
  LOG_USE_MODULE(AvPlayer);

  std::unique_lock lock(m_mutex_int);

  auto mapped = accessFileManager().getMappedPath(std::string(filename));
  if (!mapped) {
    return false;
  }
  m_filename = mapped.value();

  if (!std::filesystem::exists(m_filename)) {
    LOG_ERR(L"File doesn't exist:%S -> mapped:%s", filename, m_filename.c_str());
    return false;
  }

  // Init ffmpeg
  if (avformat_open_input(&m_pFmtCtx, m_filename.string().c_str(), NULL, NULL) < 0) {
    LOG_ERR(L"avformat_open_input %s", filename, m_filename.c_str());
    return false;
  }

  if (avformat_find_stream_info(m_pFmtCtx, NULL) < 0) {
    LOG_ERR(L"avformat_find_stream_info %s", filename, m_filename.c_str());
    avformat_close_input(&m_pFmtCtx);
    return false;
  }

  m_isVideoActive = false;
  m_isAudioActive = false;
  if (openCodecContext(&m_video.streamIndex, m_pFmtCtx, AVMEDIA_TYPE_VIDEO, &m_video.codecContext)) {
    m_isVideoActive = true;
    m_startTime     = 0;

    m_video.frame = av_frame_alloc();

    m_video.stream = m_pFmtCtx->streams[m_video.streamIndex];
    LOG_DEBUG(L"video stream: %u:%u rate:%f", m_video.codecContext->width, m_video.codecContext->height, av_q2d(m_video.codecContext->framerate));

    auto const videobufferSize = av_image_get_buffer_size(AV_PIX_FMT_NV12, m_video.codecContext->width, m_video.codecContext->height, 1);
    if (videobufferSize < 0) {
      LOG_ERR(L"av_image_get_buffer_size %s", m_filename.c_str());
      destroyAVData();
      return false;
    }

    uint64_t const alignment = getImageAlignment(VK_FORMAT_R8G8_UNORM, VkExtent3D {1920, 1080, 1}); // image size doesn't matter
    m_videoBuffer            = (void*)((uint64_t)m_memAlloc.allocateTexture(m_memAlloc.objectPointer, alignment, videobufferSize));

    m_videoStride = std::array<int, 4> {m_video.codecContext->width, m_video.codecContext->width, 0, 0};
    m_videoPlane  = std::array<void*, 4> {m_videoBuffer, (uint8_t*)m_videoBuffer + m_video.codecContext->width * m_video.codecContext->height, 0, 0};
  }

  if (openCodecContext(&m_audio.streamIndex, m_pFmtCtx, AVMEDIA_TYPE_AUDIO, &m_audio.codecContext)) {
    m_isAudioActive = true;
    if (swr_alloc_set_opts2(&m_swrCtx, &m_audio.codecContext->ch_layout, AVSampleFormat::AV_SAMPLE_FMT_S16, m_audio.codecContext->sample_rate,
                            &m_audio.codecContext->ch_layout, m_audio.codecContext->sample_fmt, m_audio.codecContext->sample_rate, 0, NULL)) {
      LOG_ERR(L"Couldn't alloc swr");
    }
    swr_init(m_swrCtx);

    m_audio.frame  = av_frame_alloc();
    m_audio.stream = m_pFmtCtx->streams[m_audio.streamIndex];
    LOG_DEBUG(L"audio stream: rate:%d channels:%d", m_audio.codecContext->sample_rate, m_audio.codecContext->ch_layout.nb_channels);

    m_audioBuffer.clear();
  }
  // -ffmpeg

  LOG_INFO(L"added:%S -> mapped:%s", filename, m_filename.c_str());

  m_decodeThread = threadFunc();
  return true;
}

bool Avplayer::getVideoData(void* info, bool isEx) {
  LOG_USE_MODULE(AvPlayer);

  if (m_isVideoActive == false) return false;

  // Get Frame from decoder
  while (m_video.getNewFrame) {
    std::unique_lock lock(m_mutexDecode);

    auto const retRecv = avcodec_receive_frame(m_video.codecContext, m_video.frame);
    if (retRecv < 0) {
      if (retRecv == AVERROR(EAGAIN)) {
        if (!m_isStop) {
          m_video.m_cond.wait(lock);
          continue;
        }
      } else if (retRecv != AVERROR_EOF) {
        LOG_ERR(L"avcodec_receive_frame:%d", retRecv);
      }
      LOG_INFO(L"Video EOF");
      m_isVideoActive = false; // End of file

      m_video.getNewFrame = false;
      m_video.pending     = false;
      m_condDecode.notify_one();
      return false;
    }

    m_video.getNewFrame = false;
  }
  // -frame

  // set startTime if first call
  if (m_startTime == 0) {
    // if audio is available -> audio sets first timestamp
    if (m_isAudioActive) return false;

    m_startTime = av_gettime();
  }
  // - start time

  auto const timestamp = (int64_t)(1000.0 * av_q2d(m_video.stream->time_base) * m_video.frame->best_effort_timestamp); // timestamp[seconds] to [ms]

  auto const curTime = (av_gettime() - m_startTime) / 1000; // [us] to [ms]
  if (timestamp > curTime) {
    return false;
  }

  LOG_DEBUG(L"Received video frame, timestamp:%lld", m_video.frame->pts);
  m_video.getNewFrame = true;

  //  Convert and copy to texture

  m_swsCtx = sws_getCachedContext(m_swsCtx, m_video.frame->width, m_video.frame->height, m_video.codecContext->pix_fmt, m_video.frame->width,
                                  m_video.frame->height, AV_PIX_FMT_NV12, NULL, NULL, NULL, NULL);
  sws_scale(m_swsCtx, m_video.frame->data, m_video.frame->linesize, 0, m_video.frame->height, (uint8_t**)m_videoPlane.data(), m_videoStride.data());
  // -

  av_frame_unref(m_video.frame);

  {
    std::unique_lock lock(m_mutexDecode);
    if (m_video.pending) {
      m_video.pending = false;
      LOG_DEBUG(L"getAudioData: notfiy decoder");
      lock.unlock();
      m_condDecode.notify_one();
    }
  }

  // Set Info
  if (isEx) {
    auto* pInfo      = (SceAvPlayerFrameInfoEx*)info;
    pInfo->timeStamp = timestamp;
    pInfo->pData     = m_videoBuffer;

    pInfo->details.video.width           = m_video.codecContext->width;
    pInfo->details.video.height          = m_video.codecContext->height;
    pInfo->details.video.aspectRatio     = av_q2d(m_video.codecContext->sample_aspect_ratio);
    pInfo->details.video.languageCode[0] = 'e';
    pInfo->details.video.languageCode[1] = 'n';
    pInfo->details.video.languageCode[2] = 'g';
    pInfo->details.video.languageCode[3] = '\0';

    pInfo->details.video.framerate          = av_q2d(m_video.codecContext->framerate);
    pInfo->details.video.cropLeftOffset     = 0;
    pInfo->details.video.cropRightOffset    = 0;
    pInfo->details.video.cropTopOffset      = 0;
    pInfo->details.video.cropBottomOffset   = 0;
    pInfo->details.video.pitch              = m_video.codecContext->width;
    pInfo->details.video.lumaBitDepth       = 8;
    pInfo->details.video.chromaBitDepth     = 8;
    pInfo->details.video.videoFullRangeFlag = 0;
  } else {
    auto* pInfo      = (SceAvPlayerFrameInfo*)info;
    pInfo->timeStamp = timestamp;
    pInfo->pData     = (uint8_t*)m_videoBuffer;

    pInfo->details.video.width           = m_video.codecContext->width;
    pInfo->details.video.height          = m_video.codecContext->height;
    pInfo->details.video.aspectRatio     = av_q2d(m_video.codecContext->sample_aspect_ratio);
    pInfo->details.video.languageCode[0] = 'e';
    pInfo->details.video.languageCode[1] = 'n';
    pInfo->details.video.languageCode[2] = 'g';
    pInfo->details.video.languageCode[3] = '\0';
  }
  // -info
  return true;
}

bool Avplayer::getAudioData(SceAvPlayerFrameInfo* info) {
  LOG_USE_MODULE(AvPlayer);
  if (m_isAudioActive == false) return false;

  // Get Frame from decoder
  while (true) {
    std::unique_lock lock(m_mutexDecode);

    auto const retRecv = avcodec_receive_frame(m_audio.codecContext, m_audio.frame);
    if (retRecv < 0) {
      if (retRecv == AVERROR(EAGAIN)) {
        if (!m_isStop) {
          m_audio.m_cond.wait(lock);
          continue;
        }
      } else if (retRecv != AVERROR_EOF) {
        LOG_ERR(L"avcodec_receive_frame:%d", retRecv);
      }
      LOG_INFO(L"Audio EOF");
      m_isAudioActive = false; // End of file

      m_audio.pending = false;
      m_condDecode.notify_one();
      return false;
    }
    break;
  }
  // -Frame

  LOG_DEBUG(L"Received audio frame %lld", m_audio.frame->pts);

  if (m_startTime == 0) {
    // audio sets first timestamp
    m_startTime = av_gettime();
  }

  auto const timestamp = (int64_t)(1000.0 * av_q2d(m_audio.stream->time_base) * m_audio.frame->best_effort_timestamp); // timestamp[seconds] to [ms]

  //  Convert and copy
  int const outNumSamples = swr_get_out_samples(m_swrCtx, m_audio.frame->nb_samples);
  if (m_audioBuffer.size() == 0) {
    auto const bufferSize = m_audio.frame->ch_layout.nb_channels * outNumSamples * av_get_bytes_per_sample((AVSampleFormat)m_audio.frame->format);
    m_audioBuffer.resize(bufferSize);
  }

  uint8_t* audioBuffers[1] = {m_audioBuffer.data()};
  if (swr_convert(m_swrCtx, audioBuffers, outNumSamples, (uint8_t const**)m_audio.frame->extended_data, m_audio.frame->nb_samples) < 0) {
    LOG_ERR(L"swr_convert");
  }
  // -

  av_frame_unref(m_audio.frame);
  {
    std::unique_lock lock(m_mutexDecode);
    if (m_audio.pending) {
      m_audio.pending = false;
      LOG_DEBUG(L"getAudioData: notfiy decoder");
      lock.unlock();
      m_condDecode.notify_one();
    }
  }

  info->timeStamp = timestamp;
  info->pData     = m_audioBuffer.data();

  info->details.audio.channelCount    = m_audio.codecContext->ch_layout.nb_channels;
  info->details.audio.sampleRate      = m_audio.codecContext->sample_rate;
  info->details.audio.size            = m_audioBuffer.size();
  info->details.audio.languageCode[0] = 'e';
  info->details.audio.languageCode[1] = 'n';
  info->details.audio.languageCode[2] = 'g';
  info->details.audio.languageCode[3] = '\0';
  return true;
}

std::unique_ptr<std::thread> Avplayer::threadFunc() {
  m_isStop = false;

  return std::make_unique<std::thread>([this] {
    LOG_USE_MODULE(AvPlayer);
    util::setThreadName("AvPlayer Decoder");

    m_video.pending = false;
    m_audio.pending = false;

    AVPacket* packet = nullptr;

    bool isEOF = false;

    std::deque<AVPacket*> packetQueue;
    size_t                countPackets = 0;

    auto sendFunc = [&packetQueue, this](DataBase& curData) {
      LOG_USE_MODULE(AvPlayer);

      if (curData.pending) return; // Wait on client

      while (!curData.decodeQueue.empty()) {
        auto packet = curData.decodeQueue.front();
        {
          std::unique_lock lock(m_mutexDecode);
          if (int ret = avcodec_send_packet(curData.codecContext, packet); ret < 0) {
            if (ret == AVERROR(EAGAIN)) {
              curData.pending = true;
              LOG_DEBUG(L"avcodec_send_packet: decoder full");
            } else {
              LOG_ERR(L"avcodec_send_packet");
            }
            return;
          }
        }
        curData.m_cond.notify_one(); // Notify client
        curData.decodeQueue.pop();

        LOG_DEBUG(L"##Send Packet %d queue:%llu time:%lld", packet->stream_index, curData.decodeQueue.size(), packet->pts);
        av_packet_unref(packet);
        packetQueue.push_back(packet);
      }
    };

    while (!m_isStop) {
      // Get New Frames

      if (!isEOF) {
        if (packetQueue.empty()) {
          packet = av_packet_alloc();
          ++countPackets;
        } else {
          packet = packetQueue.front();
          packetQueue.pop_front();
        }
        LOG_DEBUG(L"Read new Frame: queue:%llu total:%llu", packetQueue.size(), countPackets);

        int state = av_read_frame(m_pFmtCtx, packet);
        if (state < 0) {
          if (state != AVERROR_EOF) {
            LOG_ERR(L"av_read_frame error %d", state);
            break;
          } else {
            if (m_isLoop) {
              LOG_ERR(L"todo video loop");
            }
            isEOF = true;
            LOG_INFO(L"av_read_frame eof");
          }
        }
        if (packet->stream_index == m_video.streamIndex) {
          m_video.decodeQueue.push(packet);
          LOG_DEBUG(L"Queue Video Packet: numItems:%llu pts:%lld", m_video.decodeQueue.size(), packet->pts);
        } else if (packet->stream_index == m_audio.streamIndex) {
          m_audio.decodeQueue.push(packet);
          LOG_DEBUG(L"Queue Video Packet: numItems:%llu pts:%lld", m_video.decodeQueue.size(), packet->pts);
        } else
          continue;
      }
      // - new frames

      // Decode Frame
      while (true) {
        sendFunc(m_video);
        sendFunc(m_audio);

        std::unique_lock lock(m_mutexDecode);
        if (m_video.pending && !m_video.decodeQueue.empty() && m_audio.pending && !m_audio.decodeQueue.empty()) {
          m_condDecode.wait(lock, [&] { return m_video.pending; });
          continue;
        }
        break;
      }

      if (isEOF && m_video.decodeQueue.empty() && m_audio.decodeQueue.empty()) {
        m_isStop = true;
        m_video.m_cond.notify_one();
        m_audio.m_cond.notify_one();
        break;
      }
    }

    LOG_INFO(L"Decoder exit: packets:%llu", packetQueue.size());
    for (auto item: packetQueue) {
      av_packet_free(&packet);
    }
  });
}