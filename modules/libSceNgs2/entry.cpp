extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
#include "core/fileManager/ifile.h"
#include "core/kernel/filesystem.h"
#include "core/kernel/pthread.h"
#include "logging.h"
#include "readFuncs.h"
#include "reader.h"
#include "types.h"

#include <boost/thread.hpp>

LOG_DEFINE_MODULE(libSceNgs2);

namespace {
static boost::mutex MUTEX_INT;

int32_t _voiceControlWaveformBlock(SceNgs2Handle* voh, const SceNgs2SamplerVoiceWaveformBlocksParam* svwfbp) {
  LOG_USE_MODULE(libSceNgs2);

  LOG_TRACE(L"waveblock: %d\n", svwfbp->numBlocks);
  LOG_TRACE(L"waveptr: %llx\n", svwfbp->data);

  if (voh->type != SceNgs2HandleType::Voice) return Err::Ngs2::INVALID_VOICE_HANDLE;
  auto voice = (SceNgs2Handle_voice*)voh;

  voice->reader = std::make_unique<Reader>().release();
  voice->reader->init(svwfbp->data, svwfbp->aBlock[0].size);

  return Ok;
}

int32_t voiceControl_voice(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  switch ((SceNgs2VoiceParam)(phead->id & 0xFFFF)) {
    case SceNgs2VoiceParam::SET_MATRIX_LEVELS: {
    } break;
    case SceNgs2VoiceParam::SET_PORT_VOLUME: {
    } break;
    case SceNgs2VoiceParam::SET_PORT_MATRIX: {
    } break;
    case SceNgs2VoiceParam::SET_PORT_DELAY: {
    } break;
    case SceNgs2VoiceParam::PATCH: {
    } break;
    case SceNgs2VoiceParam::KICK_EVENT: {
    } break;
    case SceNgs2VoiceParam::SET_CALLBACK: {
    } break;
  }
  return Ok;
}

int32_t voiceControl_mastering(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  switch ((SceNgs2MasteringParam)(phead->id & 0xFFFF)) {
    case SceNgs2MasteringParam::SETUP: {
    } break;
    case SceNgs2MasteringParam::SET_MATRIX: {
    } break;
    case SceNgs2MasteringParam::SET_LFE: {
    } break;
    case SceNgs2MasteringParam::SET_LIMITER: {
    } break;
    case SceNgs2MasteringParam::SET_GAIN: {
    } break;
    case SceNgs2MasteringParam::SET_OUTPUT: {
    } break;
    case SceNgs2MasteringParam::SET_PEAK_METER: {
    } break;
  }
  return Ok;
}

int32_t voiceControl_sampler(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  switch ((SceNgs2SamplerParam)(phead->id & 0xFFFF)) {
    case SceNgs2SamplerParam::SETUP: {
    } break;
    case SceNgs2SamplerParam::ADD_WAVEFORM_BLOCKS: {
      return _voiceControlWaveformBlock(voh, (const SceNgs2SamplerVoiceWaveformBlocksParam*)phead);
    } break;
    case SceNgs2SamplerParam::REPLACE_WAVEFORM_ADDRESS: {
    } break;
    case SceNgs2SamplerParam::SET_WAVEFORM_FRAME_OFFSET: {
    } break;
    case SceNgs2SamplerParam::EXIT_LOOP: {
    } break;
    case SceNgs2SamplerParam::SET_PITCH: {
    } break;
    case SceNgs2SamplerParam::SET_ENVELOPE: {
    } break;
    case SceNgs2SamplerParam::SET_DISTORTION: {
    } break;
    case SceNgs2SamplerParam::SET_USER_FX: {
    } break;
    case SceNgs2SamplerParam::SET_PEAK_METER: {
    } break;
    case SceNgs2SamplerParam::SET_FILTER: {
    } break;
  }
  return Ok;
}

int32_t voiceControl_submixer(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  switch ((SceNgs2SubmixerParam)(phead->id & 0xFFFF)) {
    case SceNgs2SubmixerParam::SETUP: {
    } break;
    case SceNgs2SubmixerParam::SET_ENVELOPE: {
    } break;
    case SceNgs2SubmixerParam::SET_COMPRESSOR: {
    } break;
    case SceNgs2SubmixerParam::SET_DISTORTION: {
    } break;
    case SceNgs2SubmixerParam::SET_USER_FX: {
    } break;
    case SceNgs2SubmixerParam::SET_PEAK_METER: {
    } break;
    case SceNgs2SubmixerParam::SET_FILTER: {
    } break;
  }
  return Ok;
}
} // namespace

SceNgs2Handle_voice::~SceNgs2Handle_voice() {
  if (reader != nullptr) delete reader;
}

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNgs2";

// ### Create
EXPORT SYSV_ABI int32_t sceNgs2RackQueryBufferSize(uint32_t rackId, const SceNgs2RackOption* ro, SceNgs2ContextBufferInfo* cbi) {
  if (cbi == nullptr) return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  if (ro != nullptr && ro->size < sizeof(SceNgs2RackOption)) return Err::Ngs2::INVALID_OPTION_SIZE;

  auto const numVoices = ro != nullptr ? ro->maxVoices : SceNgs2RackOption().maxVoices;
  cbi->hostBufferSize  = sizeof(SceNgs2Handle_rack) + (numVoices * sizeof(SceNgs2Handle_voice));
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemQueryBufferSize(const SceNgs2SystemOption* sysopt, SceNgs2ContextBufferInfo* cbi) {
  if (cbi == nullptr) return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  cbi->hostBufferSize = sizeof(SceNgs2Handle_system);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreateWithAllocator(const SceNgs2SystemOption* sysopt, SceNgs2BufferAllocator* alloc, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);

  boost::unique_lock lock(MUTEX_INT);

  if (alloc == nullptr || alloc->allocHandler == nullptr) return Err::Ngs2::INVALID_BUFFER_ALLOCATOR;
  if (outh == nullptr) return Err::Ngs2::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size < sizeof(SceNgs2SystemOption)) return Err::Ngs2::INVALID_OPTION_SIZE;

  SceNgs2ContextBufferInfo cbi = {
      .hostBuffer     = nullptr,
      .hostBufferSize = sizeof(SceNgs2Handle_system),
      .userData       = alloc->userData,
  };

  if (auto ret = alloc->allocHandler(&cbi)) {
    LOG_ERR(L"Ngs2System: Allocation failed!");
    return ret;
  }

  *outh = new (cbi.hostBuffer) SceNgs2Handle_system(alloc);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreate(const SceNgs2SystemOption* sysopt, const SceNgs2ContextBufferInfo* cbi, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  boost::unique_lock lock(MUTEX_INT);

  if (outh == nullptr) return Err::Ngs2::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size < sizeof(SceNgs2SystemOption)) return Err::Ngs2::INVALID_OPTION_SIZE;
  if (cbi == nullptr || cbi->hostBuffer == nullptr || cbi->hostBufferSize < sizeof(SceNgs2Handle)) return Err::Ngs2::INVALID_BUFFER_ADDRESS;

  *outh = new (cbi->hostBuffer) SceNgs2Handle_system(nullptr);

  return (*outh) != nullptr ? Ok : Err::Ngs2::FAIL;
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreate(SceNgs2Handle* sysh, uint32_t rackId, const SceNgs2RackOption* ropt, const SceNgs2ContextBufferInfo* cbi,
                                          SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  boost::unique_lock lock(MUTEX_INT);

  if (outh == nullptr) return Err::Ngs2::INVALID_OUT_ADDRESS;
  if (sysh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (ropt != nullptr && ropt->size < sizeof(SceNgs2RackOption)) return Err::Ngs2::INVALID_OPTION_SIZE;
  if (cbi == nullptr || cbi->hostBuffer == nullptr || cbi->hostBufferSize < sizeof(SceNgs2Handle)) return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  auto system = (SceNgs2Handle_system*)sysh;

  *outh = new (cbi->hostBuffer) SceNgs2Handle_rack(system, ropt, rackId);

  auto rack = (SceNgs2Handle_rack*)(*outh);

  // Init voices
  {
    auto addrVoices = (uint64_t)rack + sizeof(SceNgs2Handle_rack); // At end of rack struct

    rack->voices = (SceNgs2Handle_voice*)addrVoices;

    for (uint16_t n = 0; n < rack->options.maxVoices; ++n, addrVoices += sizeof(SceNgs2Handle_voice)) {
      auto pVoice = new ((void*)addrVoices) SceNgs2Handle_voice(rack);
    }
  }
  // -

  // Add to system
  if (rackId == SCE_NGS2_RACK_ID_MASTERING || rackId == SCE_NGS2_RACK_ID_CUSTOM_MASTERING) {
    system->mastering = rack;
  } else if (rackId == SCE_NGS2_RACK_ID_SAMPLER || rackId == SCE_NGS2_RACK_ID_CUSTOM_SAMPLER) {
    system->sampler = rack;
  } else if (rackId == SCE_NGS2_RACK_ID_SUBMIXER || rackId == SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER) {
    system->submixer = rack;
  } else {
    LOG_ERR(L"-> Rack: undefined rackid %u", rackId);
    return Err::Ngs2::FAIL;
  }

  return (*outh) != nullptr ? Ok : Err::Ngs2::FAIL;
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreateWithAllocator(SceNgs2Handle* sysh, uint32_t rackId, const SceNgs2RackOption* ro, const SceNgs2BufferAllocator* alloc,
                                                       SceNgs2Handle** outh) {
  if (sysh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (alloc == nullptr) return Err::Ngs2::INVALID_BUFFER_ALLOCATOR;
  LOG_USE_MODULE(libSceNgs2);
  boost::unique_lock lock(MUTEX_INT);

  auto system = (SceNgs2Handle_system*)sysh;

  auto const numVoices = ro != nullptr ? ro->maxVoices : SceNgs2RackOption().maxVoices;

  SceNgs2ContextBufferInfo cbi = {
      .hostBuffer     = nullptr,
      .hostBufferSize = sizeof(SceNgs2Handle_rack) + (numVoices * sizeof(SceNgs2Handle_voice)),
      .userData       = alloc->userData,
  };

  if (auto ret = alloc->allocHandler(&cbi)) {
    LOG_ERR(L"Ngs2Rack: Allocation failed!");
    return ret;
  }

  *outh = new (cbi.hostBuffer) SceNgs2Handle_rack(system, ro, rackId);

  auto rack = (SceNgs2Handle_rack*)(*outh);
  // Init voices
  {
    auto addrVoices = (uint64_t)rack + sizeof(SceNgs2Handle_rack); // At end of rack struct

    rack->voices = (SceNgs2Handle_voice*)addrVoices;

    for (uint16_t n = 0; n < rack->options.maxVoices; ++n, addrVoices += sizeof(SceNgs2Handle_voice)) {
      auto pVoice = new ((void*)addrVoices) SceNgs2Handle_voice(rack);
    }
  }
  // -

  // Add to system
  if (rackId == SCE_NGS2_RACK_ID_MASTERING || rackId == SCE_NGS2_RACK_ID_CUSTOM_MASTERING) {
    system->mastering = rack;
  } else if (rackId == SCE_NGS2_RACK_ID_SAMPLER || rackId == SCE_NGS2_RACK_ID_CUSTOM_SAMPLER) {
    system->sampler = rack;
  } else if (rackId == SCE_NGS2_RACK_ID_SUBMIXER || rackId == SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER) {
    system->submixer = rack;
  } else {
    LOG_ERR(L"-> Rack: undefined rackid %u", rackId);
    return Err::Ngs2::FAIL;
  }

  return (*outh) != nullptr ? Ok : Err::Ngs2::FAIL;
}

// --- create

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

// ### WAVE File parsing

/**
 * @brief Parse the wave (file) data
 *
 * @param ptr Game/Application reads (file) and exposes the data pointer
 * @param size size of data(file)
 * @param wf
 * @return result
 */
EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformData(const void* ptr, size_t size, SceNgs2WaveformFormat* wf) {
  if (ptr == nullptr) {
    return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  }
  boost::unique_lock lock(MUTEX_INT);

  userData_inerBuffer userData {ptr, size, 0};
  return parseWave(readFunc_linearBuffer, seekFunc_linearBuffer, &userData, wf);
}

/**
 * @brief Parse the wave file
 *
 * @param ptr Game/Application reads file and exposes the data pointer
 * @param offset offset of file
 * @param wf
 * @return result
 */

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformFile(const char* path, long offset, SceNgs2WaveformFormat* wf) {
  boost::unique_lock lock(MUTEX_INT);

  filesystem::SceOpen const fileFlags {
      .mode = filesystem::SceOpenMode::RDONLY,
  };

  auto fileHandle = filesystem::open(path, fileFlags, 0);
  if (fileHandle < 0) return Err::Ngs2::INVALID_WAVEFORM_DATA;

  if (offset != 0) filesystem::lseek(fileHandle, offset, (int)SceWhence::beg);
  return parseWave(readFunc_file, seekFunc_file, reinterpret_cast<void*>(fileHandle), wf);
}

/**
 * @brief Parse the wave file using the provided function
 *
 * @param func Game/Application reads file and exposes the data pointer
 * @param userData_ offset of file
 * @param wf
 * @return result
 */

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformUser(SceWaveformUserFunc func, uintptr_t userData_, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  // todo parseWave uses seek for totalsize
  return Err::Ngs2::FAIL;

  // if (func == nullptr) {
  //   return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  // }
  // boost::unique_lock lock(MUTEX_INT);

  // userData_user userData {func, userData_, 0};
  // return parseWave(readFunc_user, seekFunc_user, (void*)&userData, wf);
}

// - wave parsing

EXPORT SYSV_ABI int32_t sceNgs2GetWaveformFrameInfo(const SceNgs2WaveformInfo* fmt, uint32_t* outFrameSize, uint32_t* outNumFrameSamples,
                                                    uint32_t* outUnitsPerFrame, uint32_t* outNumDelaySamples) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);

  boost::unique_lock lock(MUTEX_INT);

  if (outFrameSize) *outFrameSize = 1; // Some games crashes with divide by zero exception if we set 0 here
  if (outNumFrameSamples) *outNumFrameSamples = 1;
  if (outUnitsPerFrame) *outUnitsPerFrame = 1;
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
  LOG_USE_MODULE(libSceNgs2);
  boost::unique_lock lock(MUTEX_INT);

  if (rh == nullptr || rh->type != SceNgs2HandleType::Rack) return Err::Ngs2::INVALID_RACK_HANDLE;

  auto rack = (SceNgs2Handle_rack*)rh;
  if (voiceId > rack->options.maxVoices) return Err::Ngs2::INVALID_VOICE_INDEX;

  *outh = (SceNgs2Handle*)((uint64_t)rack->voices + voiceId * sizeof(SceNgs2Handle_voice));

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
  boost::unique_lock lock(MUTEX_INT);

  // if (sysh == nullptr || sysh->un.rack.voices == nullptr || sysh->un.rack.voices->un.voice.reader == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  if (rbi->bufferPtr == nullptr) return Err::Ngs2::INVALID_BUFFER_ADDRESS;
  if (rbi->bufferSize == 0) return Err::Ngs2::INVALID_BUFFER_SIZE;
  if (rbi->waveType >= SceNgs2WaveFormType::MAX_TYPES) return Err::Ngs2::INVALID_WAVEFORM_TYPE;
  if (rbi->channelsCount > SceNgs2ChannelsCount::CH_7_1) return Err::Ngs2::INVALID_NUM_CHANNELS;

  auto system = (SceNgs2Handle_system*)sysh;

  if (system->sampler == nullptr) {
    for (int32_t i = 0; i < count; i++) {
      if (rbi[i].bufferPtr != nullptr) {
        std::memset(rbi[i].bufferPtr, 0, rbi[i].bufferSize);
      }
    }

  } else {
    for (int32_t i = 0; i < count; i++) {
      if (rbi[i].bufferPtr != nullptr) {
        auto reader = system->sampler->voices[0].reader;
        if (reader != nullptr) {
          reader->getAudio(rbi[i].bufferPtr, rbi[i].bufferSize);
        } else {
          std::memset(rbi[i].bufferPtr, 0, rbi[i].bufferSize);
        }
      }
    }
  }
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

EXPORT SYSV_ABI int32_t sceNgs2VoiceControl(SceNgs2Handle* voh, const SceNgs2VoiceParamHead* phead) {
  LOG_USE_MODULE(libSceNgs2);
  boost::unique_lock lock(MUTEX_INT);

  uint16_t const rackId = phead->id >> 16;

  printf("0x%u 0x%u\n", rackId, (uint16_t)(phead->id & 0xffff)); // todo racecondition somewhere
  if (rackId == SCE_NGS2_RACK_ID_VOICE) return voiceControl_voice(voh, phead);
  if (rackId == SCE_NGS2_RACK_ID_MASTERING || rackId == SCE_NGS2_RACK_ID_CUSTOM_MASTERING) return voiceControl_mastering(voh, phead);
  if (rackId == SCE_NGS2_RACK_ID_SAMPLER || rackId == SCE_NGS2_RACK_ID_CUSTOM_SAMPLER) return voiceControl_sampler(voh, phead);
  if (rackId == SCE_NGS2_RACK_ID_SUBMIXER || rackId == SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER) return voiceControl_submixer(voh, phead);

  LOG_ERR(L"Unhandled rackId: %08x", rackId);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetState(SceNgs2Handle* voh, SceNgs2VoiceState* state, size_t size) {
  if (voh == nullptr || state == nullptr) return getErr(ErrCode::_EINVAL);
  if (voh->type != SceNgs2HandleType::Voice) return Err::Ngs2::INVALID_VOICE_HANDLE;

  auto voice        = (SceNgs2Handle_voice*)voh;
  state->stateFlags = voice->state.stateFlags;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetStateFlags(SceNgs2Handle* voh, uint32_t* flags) {
  if (voh == nullptr || flags == nullptr) return getErr(ErrCode::_EINVAL);

  if (voh->type != SceNgs2HandleType::Voice) return Err::Ngs2::INVALID_VOICE_HANDLE;

  auto voice = (SceNgs2Handle_voice*)voh;
  *flags     = voice->state.stateFlags;
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

EXPORT SYSV_ABI int32_t sceNgs2RackDestroy(SceNgs2Handle* rh, SceNgs2ContextBufferInfo* cbi) {
  if (rh == nullptr) return Err::Ngs2::INVALID_RACK_HANDLE;
  boost::unique_lock lock(MUTEX_INT);

  auto rack = (SceNgs2Handle_rack*)rh;

  auto freeHandler = rack->parent->alloc.freeHandler;

  int32_t ret = Ok;
  if (freeHandler != nullptr) {
    SceNgs2ContextBufferInfo cbi = {
        .hostBuffer     = rack,
        .hostBufferSize = sizeof(SceNgs2Handle_rack) + (rack->options.maxVoices * sizeof(SceNgs2Handle_voice)),
    };
    rack->~SceNgs2Handle_rack();
    ret = freeHandler(&cbi);
  } else
    rack->~SceNgs2Handle_rack();

  return ret;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemDestroy(SceNgs2Handle* sysh, SceNgs2ContextBufferInfo* cbi) {
  if (sysh == nullptr) return Err::Ngs2::INVALID_SYSTEM_HANDLE;
  boost::unique_lock lock(MUTEX_INT);

  auto system = (SceNgs2Handle_system*)sysh;

  auto freeHandler = system->alloc.freeHandler;

  int32_t ret = Ok;
  if (freeHandler != nullptr) {
    SceNgs2ContextBufferInfo cbi = {
        .hostBuffer     = system,
        .hostBufferSize = sizeof(SceNgs2Handle_system),
    };
    system->~SceNgs2Handle_system();
    ret = freeHandler(&cbi);
  } else
    system->~SceNgs2Handle_system();

  return ret;
}
}
