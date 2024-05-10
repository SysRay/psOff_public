#include "readFuncs.h"

#include "core/fileManager/ifile.h"
#include "core/kernel/filesystem.h"
#include "logging.h"
#include "riffTypes.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <functional>
#include <list>
#include <sstream>

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

  int64_t const dataLeft = (int64_t)userData->size - (int64_t)userData->curOffset;

  auto readSize = std::min(dataLeft, (int64_t)size);

  if (readSize > 0) {
    ::memcpy(buf, (uint8_t*)userData->ptr + userData->curOffset, readSize);
    userData->curOffset += readSize;
    return readSize;
  }

  userData->curOffset = 0; // reset it
  return -1;
}

int64_t seekFunc_linearBuffer(void* userData_, int64_t offset, int whence) {
  auto userData = (userData_inerBuffer*)userData_;

  if ((whence & AVSEEK_SIZE) > 0) {
    // return size (0 on not avail)
    return userData->size;
  }

  switch ((std::ios_base::seekdir)(whence & 0xff)) {
    case std::ios_base::beg: userData->curOffset = offset; break;
    case std::ios_base::cur:
      if (userData->curOffset < offset)
        userData->curOffset = 0;
      else
        userData->curOffset += offset;
      break;
    case std::ios_base::end: userData->curOffset = std::min(userData->size, userData->size + offset); break;
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

int32_t parseRiffWave(funcReadBuf_t readFunc, funcSeekBuf_t seekFunc, void* userData, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);

  // Load headers and check magic
  // todo use ffmeg, get the correct type (pcm, at9...)

  // Check if correct file
  RiffWaveHeader riffHeader;
  readFunc(userData, (uint8_t*)&riffHeader, sizeof(RiffWaveHeader));
  if (memcmp(riffHeader.chunkID, "RIFF", 4) != 0 || memcmp(riffHeader.riffType, "WAVE", 4) != 0) {
    LOG_ERR(L"wrong riff 0x%lx 0x%lx", riffHeader.chunkID, riffHeader.riffType);
    return Err::Ngs2::FAIL;
  }
  // -

  // Format header
  RiffFormatHeader formatHeader;
  readFunc(userData, (uint8_t*)&formatHeader, sizeof(RiffFormatHeader));

  if (memcmp(formatHeader.chunkID, "fmt ", 4) != 0) {
    return Err::Ngs2::FAIL;
  }

  if ((8 + formatHeader.chunkSize) > sizeof(RiffFormatHeader)) {
    std::vector<uint8_t> extraFmt(8 + formatHeader.chunkSize - sizeof(RiffFormatHeader));
    readFunc(userData, extraFmt.data(), extraFmt.size());
  }
  // -

  // parse rest
  uint32_t offset = sizeof(RiffWaveHeader) + 8 + formatHeader.chunkSize;

  uint32_t dataSize     = 0;
  uint32_t numOfSamples = 1;

  // Data header
  while (offset < riffHeader.chunkSize) {
    RiffHeader header;
    readFunc(userData, (uint8_t*)&header, sizeof(RiffHeader));

    auto readBytes = sizeof(RiffHeader);
    if (memcmp(header.chunkID, "fact", 4) == 0) {
      readFunc(userData, (uint8_t*)&numOfSamples, 4);
      readBytes += 4;
    } else if (memcmp(header.chunkID, "data", 4) == 0) {
      dataSize = header.chunkSize;
      break;
    } // Dump read data

    if ((8 + header.chunkSize) > readBytes) {
      std::vector<uint8_t> extra(8 + header.chunkSize - readBytes);
      readFunc(userData, extra.data(), extra.size());
    }
    // -

    offset += 8 + header.chunkSize;
  }
  if (dataSize == 0) return Err::Ngs2::FAIL;

  // Fill data
  wf->info.type          = (SceNgs2WaveFormType)formatHeader.audioFormat;
  wf->info.channelsCount = (SceNgs2ChannelsCount)formatHeader.numChannels;
  wf->info.sampleRate    = formatHeader.sampleRate;
  wf->info.frameOffset   = formatHeader.frameSize;
  wf->info.frameMargin   = 0; // todo

  wf->offset          = offset;
  wf->size            = dataSize;
  wf->loopBeginPos    = 0;                      // todo
  wf->loopEndPos      = 0;                      // todo
  wf->samplesCount    = 1;                      // todo
  wf->dataPerFrame    = 1;                      // todo
  wf->frameSize       = formatHeader.frameSize; // todo
  wf->numframeSamples = 1;                      // todo
  wf->samplesDelay    = 0;                      // todo

  auto& block       = wf->block[0];
  block.offset      = wf->offset;
  block.size        = dataSize;
  block.numRepeat   = 1;
  block.skipSamples = 1;
  block.numSamples  = numOfSamples;
  wf->numBlocks     = 1;

  return Ok;
}

std::optional<AVChannelLayout> convChannelLayout(SceNgs2ChannelsCount count) {
  LOG_USE_MODULE(libSceNgs2);
  switch (count) {
    case SceNgs2ChannelsCount::CH_1_0: {
      return {AV_CHANNEL_LAYOUT_MONO};
    } break;
    case SceNgs2ChannelsCount::CH_2_0: {
      return {AV_CHANNEL_LAYOUT_STEREO};
    } break;
    case SceNgs2ChannelsCount::CH_5_1: {
      return {AV_CHANNEL_LAYOUT_5POINT1};
    } break;
    case SceNgs2ChannelsCount::CH_7_1: {
      return {AV_CHANNEL_LAYOUT_CUBE};
    } break;
    default: {
      LOG_ERR(L"channel layout not set");
    } break;
  }
  return std::nullopt;
}

uint32_t getSampleBytes(SceNgs2WaveFormType type) {
  switch (type) {
    case SceNgs2WaveFormType::PCM_U8: return 1;
    case SceNgs2WaveFormType::PCM_I16LITTLE: return 2;
    case SceNgs2WaveFormType::PCM_I16BIG: return 2;
    case SceNgs2WaveFormType::PCM_I24LITTLE: return 3;
    case SceNgs2WaveFormType::PCM_I24BIG: return 3;
    case SceNgs2WaveFormType::PCM_I32LITTLE: return 4;
    case SceNgs2WaveFormType::PCM_I32BIG: return 4;
    case SceNgs2WaveFormType::PCM_F32LITTLE: return 4;
    case SceNgs2WaveFormType::PCM_F32BIG: return 4;
    case SceNgs2WaveFormType::PCM_F64LITTLE: return 8;
    case SceNgs2WaveFormType::PCM_F64BIG: return 8;
    case SceNgs2WaveFormType::VAG: return 4;
    case SceNgs2WaveFormType::ATRAC9: return 4;
    default: break;
  }
  return 2;
}