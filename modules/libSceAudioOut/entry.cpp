#include "common.h"
#include "logging.h"
#include "portaudio.h"
#include "types.h"

#include <array>
#include <chrono>
#include <mutex>

LOG_DEFINE_MODULE(libSceAudioOut);

namespace {
struct PortOut {
  bool                   open           = false;
  int                    userId         = 0;
  SceAudioOutPortType    type           = SceAudioOutPortType::MAIN;
  uint32_t               samplesNum     = 0;
  uint32_t               freq           = 0;
  SceAudioOutParamFormat format         = SceAudioOutParamFormat::FLOAT_MONO;
  uint64_t               lastOutputTime = 0;
  int                    channelsNum    = 0;
  int                    volume[8]      = {};
  PaStream*              stream         = nullptr;
};

struct Pimpl {
  std::mutex                        mutexInt;
  std::array<PortOut, PORT_OUT_MAX> portsOut;
  Pimpl() = default;
};

Pimpl* getData() {
  static Pimpl obj;
  return &obj;
}

PaError writeOut(Pimpl* pimpl, int32_t handle, const void* ptr) {
  auto& port = pimpl->portsOut[handle - 1];
  if (!port.open) return 0;

  port.lastOutputTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
  if (Pa_IsStreamActive(port.stream) != 1) Pa_StartStream(port.stream);
  return Pa_WriteStream(port.stream, ptr, port.samplesNum);
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAudioOut";

EXPORT SYSV_ABI int32_t sceAudioOutInit(void) {
  (void)getData();
  Pa_Initialize();
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutOpen(int32_t userId, SceAudioOutPortType type, int32_t index, uint32_t len, uint32_t freq, SceAudioOutParamFormat format) {
  LOG_USE_MODULE(libSceAudioOut);
  LOG_TRACE(L"%S", __FUNCTION__);
  auto pimpl = getData();

  std::unique_lock const lock(pimpl->mutexInt);

  for (int id = 0; id < pimpl->portsOut.size(); id++) {
    auto& port = pimpl->portsOut[id];
    if (port.open) continue;

    port.userId         = userId;
    port.open           = true;
    port.type           = type;
    port.samplesNum     = len;
    port.freq           = freq;
    port.format         = format;
    port.lastOutputTime = 0;

    PaSampleFormat sampleFormat;
    switch (format) {
      case SceAudioOutParamFormat::S16_MONO:
        sampleFormat     = paInt16;
        port.channelsNum = 1;
        break;
      case SceAudioOutParamFormat::FLOAT_MONO:
        sampleFormat     = paFloat32;
        port.channelsNum = 1;
        break;
      case SceAudioOutParamFormat::S16_STEREO:
        sampleFormat     = paInt16;
        port.channelsNum = 2;
        break;
      case SceAudioOutParamFormat::FLOAT_STEREO:
        sampleFormat     = paFloat32;
        port.channelsNum = 2;
        break;
      case SceAudioOutParamFormat::S16_8CH:
        sampleFormat     = paInt16;
        port.channelsNum = 8;
        break;
      case SceAudioOutParamFormat::FLOAT_8CH:
        sampleFormat     = paFloat32;
        port.channelsNum = 8;
        break;
      case SceAudioOutParamFormat::S16_8CH_STD:
        sampleFormat     = paInt16;
        port.channelsNum = 8;
        break;
      case SceAudioOutParamFormat::FLOAT_8CH_STD:
        sampleFormat     = paFloat32;
        port.channelsNum = 8;
        break;
    }

    for (int i = 0; i < port.channelsNum; i++) {
      port.volume[i] = 32768;
    }

    PaStreamParameters const outParams {
        .device                    = Pa_GetDefaultOutputDevice(), /* default output device */
        .channelCount              = port.channelsNum,
        .sampleFormat              = sampleFormat,
        .suggestedLatency          = Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency,
        .hostApiSpecificStreamInfo = NULL,
    };

    if (auto err = Pa_OpenStream(&port.stream, NULL, &outParams, freq, port.samplesNum, paNoFlag, NULL, NULL); err != Ok) {
      return err;
    }

    return id + 1;
  }
  return Err::PORT_FULL;
}

EXPORT SYSV_ABI int32_t sceAudioOutClose(int32_t handle) {
  LOG_USE_MODULE(libSceAudioOut);
  LOG_TRACE(L"%S", __FUNCTION__);
  auto pimpl = getData();

  std::unique_lock const lock(pimpl->mutexInt);

  auto& port = pimpl->portsOut[handle - 1];
  if (port.open) {
    port.open = false;
    if (Pa_IsStreamActive(port.stream) == 1) {
      Pa_CloseStream(port.stream);
    }
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutOutput(int32_t handle, const void* ptr) {
  auto pimpl = getData();

  // std::unique_lock const lock(pimpl->mutexInt);
  return writeOut(pimpl, handle, ptr);
}

EXPORT SYSV_ABI int32_t sceAudioOutSetVolume(int32_t handle, int32_t flag, int32_t* vol) {
  LOG_USE_MODULE(libSceAudioOut);
  auto pimpl = getData();

  std::unique_lock const lock(pimpl->mutexInt);

  auto& port = pimpl->portsOut[handle - 1];
  if (!port.open) return Err::INVALID_PORT;

  for (int i = 0; i < port.channelsNum; i++, flag >>= 1u) {
    bool const bit = flag & 0x1u;
    if (!bit) continue;

    int srcIndex = i;
    if (port.format == SceAudioOutParamFormat::FLOAT_8CH_STD || port.format == SceAudioOutParamFormat::S16_8CH_STD) {
      switch (i) {
        case 4: srcIndex = 6; break;
        case 5: srcIndex = 7; break;
        case 6: srcIndex = 4; break;
        case 7: srcIndex = 5; break;
        default:;
      }
    }
    port.volume[i] = vol[srcIndex];
    LOG_DEBUG(L"port.volume[%d] volume[%d] (%d)", i, srcIndex, vol[srcIndex]);
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutOutputs(SceAudioOutOutputParam* param, uint32_t num) {
  auto pimpl = getData();

  // std::unique_lock const lock(pimpl->mutexInt); // dont block, causes audio artifacts
  for (uint32_t i = 0; i < num; i++) {
    if (auto err = writeOut(pimpl, param[i].handle, param[i].ptr); err != paNoError) return err;
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetLastOutputTime(int32_t handle, uint64_t* outputTime) {
  auto pimpl = getData();

  std::unique_lock const lock(pimpl->mutexInt);

  auto& port = pimpl->portsOut[handle - 1];
  if (!port.open) return Err::INVALID_PORT;
  *outputTime = port.lastOutputTime;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutSetMixLevelPadSpk(int32_t handle, int32_t mixLevel) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetPortState(int32_t handle, SceAudioOutPortState* state) {
  auto pimpl = getData();

  std::unique_lock const lock(pimpl->mutexInt);

  auto& port = pimpl->portsOut[handle - 1];

  state->channel = port.channelsNum;
  state->volume  = 127;
  state->output  = (uint16_t)SceAudioOutStateOutput::CONNECTED_PRIMARY;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetSystemState(SceAudioOutSystemState* state) {
  state->loudness = -70.0f;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutSetSystemDebugState(SceAudioOutSystemDebugStateElement elem, SceAudioOutSystemDebugStateParam* param) {
  return Ok;
}
}