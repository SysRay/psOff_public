#include "common.h"
#include "config_emu.h"
#include "logging.h"
#include "types.h"

#include <SDL2/SDL.h>
#include <array>
#include <chrono>
#include <mutex>

LOG_DEFINE_MODULE(libSceAudioOut);

namespace {
static bool audioInited = false;

struct PortOut {
  bool                   open           = false;
  int                    userId         = 0;
  SceAudioOutPortType    type           = SceAudioOutPortType::MAIN;
  uint8_t                sampleSize     = 0;
  uint32_t               samplesNum     = 0;
  uint32_t               freq           = 0;
  SceAudioOutParamFormat format         = SceAudioOutParamFormat::FLOAT_MONO;
  uint64_t               lastOutputTime = 0;
  int                    channelsNum    = 0;
  int                    volume[8]      = {};
  SDL_AudioDeviceID      device         = 0;
  SDL_AudioFormat        sdlFormat      = AUDIO_F32;
  float                  volumeModifier = 0.5f;
  std::vector<uint8_t>   mixedAudio;
};

class PortsOut {
  private:
  std::array<PortOut, AudioOut::PORT_OUT_MAX> m_ports = {};

  static inline uint8_t rangeStart(uint16_t range) { return (range >> 8) & 0xFF; }

  static inline uint8_t rangeEnd(uint16_t range) { return range & 0xFF; }

  public:
  PortOut* GetPort(int handle) {
    if (handle < 1 || handle > m_ports.size()) return nullptr;
    auto port = &m_ports[handle - 1];
    if (port->open) return port;
    return nullptr;
  }

  PortOut* AcquirePort(SceAudioOutPortType type, int* handle) {
    uint16_t range = 0x0000;

    switch (type) {
      case SceAudioOutPortType::MAIN: range = 0x0007; break;     // 00..07
      case SceAudioOutPortType::BGM: range = 0x0808; break;      // 08..08
      case SceAudioOutPortType::VOICE: range = 0x090C; break;    // 09..12
      case SceAudioOutPortType::PERSONAL: range = 0x0D10; break; // 13..16
      case SceAudioOutPortType::PADSPK: range = 0x1114; break;   // 17..20
      case SceAudioOutPortType::AUX: range = 0x1515; break;      // 21..21
    }

    for (int i = rangeStart(range); i <= rangeEnd(range); i++) {
      auto& port = m_ports[i];
      if (port.open) continue;

      port.open           = true;
      port.type           = type;
      port.lastOutputTime = 0;
      *handle             = (i + 1);
      return &port;
    }

    return nullptr;
  }

  int32_t UnacquirePort(int handle) {
    auto port = GetPort(handle);
    if (port == nullptr) return Err::AudioOut::INVALID_PORT;

    port->open = false;
    if (port->device > 0) {
      port->mixedAudio.clear();
      SDL_CloseAudioDevice(port->device);
    }

    return Ok;
  };
};

struct Pimpl {

  boost::mutex mutexInt;
  PortsOut     portsOut;
  Pimpl() = default;
};

Pimpl* getData() {
  static Pimpl obj;
  return &obj;
}

int writeOut(Pimpl* pimpl, int32_t handle, const void* ptr) {
  if (ptr == nullptr) return 0;

  if (auto port = pimpl->portsOut.GetPort(handle)) {
    using namespace std::chrono;
    port->lastOutputTime = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

    const size_t bytesize_1ch = port->samplesNum * port->sampleSize;

    if (port->device == 0) {
      LOG_USE_MODULE(libSceAudioOut);
      LOG_TRACE(L"Nulled audio port, playing nothing (handle=%d)", handle);
      float duration = bytesize_1ch / float(port->freq * port->sampleSize);
      SDL_Delay(int(duration * 1000)); // Pretending that we playing something
      return port->samplesNum;
    }

    const size_t bytesize  = bytesize_1ch * port->channelsNum;
    const int    maxVolume = SDL_MIX_MAXVOLUME * port->volumeModifier;
    auto&        mixed     = port->mixedAudio;
    std::fill(mixed.begin(), mixed.end(), 0);

    for (size_t i = 0; i < port->channelsNum; i++) {
      auto ch_offset = i * bytesize_1ch;
      SDL_MixAudioFormat(mixed.data() + ch_offset, ((const uint8_t*)ptr) + ch_offset, port->sdlFormat, bytesize_1ch,
                         maxVolume * ((float)port->volume[i] / AudioOut::VOLUME_0DB));
    }

    if (SDL_GetAudioDeviceStatus(port->device) != SDL_AUDIO_PLAYING) SDL_PauseAudioDevice(port->device, 0);

    int ret = SDL_QueueAudio(port->device, mixed.data(), bytesize);

    while ((ret == 0) && (SDL_GetQueuedAudioSize(port->device) > bytesize * 2))
      SDL_Delay(0);

    return ret == 0 ? port->samplesNum : Err::AudioOut::TRANS_EVENT;
  }

  return Err::AudioOut::NOT_OPENED;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAudioOut";

EXPORT SYSV_ABI int32_t sceAudioOutInit(void) {
  if (audioInited) return Err::AudioOut::ALREADY_INIT;

  if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
    audioInited = true;
    (void)getData();
    return Ok;
  }

  return Err::AudioOut::OUT_OF_MEMORY;
}

EXPORT SYSV_ABI int32_t sceAudioOutOpen(int32_t userId, SceAudioOutPortType type, int32_t index, uint32_t len, uint32_t freq, SceAudioOutParamFormat format) {
  LOG_USE_MODULE(libSceAudioOut);
  LOG_TRACE(L"%S", __FUNCTION__);
  auto pimpl = getData();

  auto getDevName = [](SceAudioOutPortType type) {
    switch (type) {
      case SceAudioOutPortType::MAIN: return "Main";
      case SceAudioOutPortType::BGM: return "Background Music";
      case SceAudioOutPortType::VOICE: return "Voice";
      case SceAudioOutPortType::PERSONAL: return "Personal";
      case SceAudioOutPortType::PADSPK: return "PadSpeaker";
      case SceAudioOutPortType::AUX: return "AUX";
      default: return "Unknown";
    }
  };

  boost::unique_lock const lock(pimpl->mutexInt);

  int handle;
  if (auto port = pimpl->portsOut.AcquirePort(type, &handle)) {
    port->userId     = userId;
    port->samplesNum = len;
    port->freq       = freq;
    port->format     = format;

    switch (format) {
      case SceAudioOutParamFormat::S16_MONO:
        port->sdlFormat   = AUDIO_S16;
        port->channelsNum = 1;
        port->sampleSize  = 2;
        break;
      case SceAudioOutParamFormat::FLOAT_MONO:
        port->sdlFormat   = AUDIO_F32;
        port->channelsNum = 1;
        port->sampleSize  = 4;
        break;
      case SceAudioOutParamFormat::S16_STEREO:
        port->sdlFormat   = AUDIO_S16;
        port->channelsNum = 2;
        port->sampleSize  = 2;
        break;
      case SceAudioOutParamFormat::FLOAT_STEREO:
        port->sdlFormat   = AUDIO_F32;
        port->channelsNum = 2;
        port->sampleSize  = 4;
        break;
      case SceAudioOutParamFormat::S16_8CH:
        port->sdlFormat   = AUDIO_S16;
        port->channelsNum = 8;
        port->sampleSize  = 2;
        break;
      case SceAudioOutParamFormat::FLOAT_8CH:
        port->sdlFormat   = AUDIO_F32;
        port->channelsNum = 8;
        port->sampleSize  = 4;
        break;
      case SceAudioOutParamFormat::S16_8CH_STD:
        port->sdlFormat   = AUDIO_S16;
        port->channelsNum = 8;
        port->sampleSize  = 2;
        break;
      case SceAudioOutParamFormat::FLOAT_8CH_STD:
        port->sdlFormat   = AUDIO_F32;
        port->channelsNum = 8;
        port->sampleSize  = 4;
        break;
    }

    SDL_AudioSpec fmt {
        .freq     = static_cast<int>(freq),
        .format   = port->sdlFormat,
        .channels = static_cast<uint8_t>(port->channelsNum),
        .samples  = static_cast<uint16_t>(port->samplesNum),
        .callback = nullptr,
        .userdata = nullptr,
    };

    std::string jdname; // Device name from JSON
    const char* dname;  // Actual device name that will be passed to SDL
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::AUDIO);

    try {
      (*jData)["volume"].get_to(port->volumeModifier);
    } catch (const json::exception& e) {
      LOG_ERR(L"Invalid audio volume setting: %S", e.what());
      port->volumeModifier = 0.5f;
    }

    if (type == SceAudioOutPortType::PADSPK) {
      auto& devn = (*jData)["padspeakers"][userId - 1];

      if (devn == "[null]") {
        LOG_INFO(L"Pad speaker for user #%d is nulled!", userId);
        port->device = 0;
        return handle;
      } else {
        try {
          devn.get_to(jdname);
          dname = jdname.c_str();
        } catch (const json::exception& e) {
          LOG_ERR(L"Failed to get pad speaker audio device for user #%d, nulling it", userId);
          port->device = 0;
          return handle;
        }
      }

      for (int i = 0; i < port->channelsNum; i++) {
        port->volume[i] = AudioOut::VOLUME_0DB;
      }
    } else {
      if ((*jData)["device"] == "[default]") {
        SDL_AudioSpec fmt_curr;
        SDL_GetDefaultAudioInfo((char**)&dname, &fmt_curr, 0);
      } else if ((*jData)["device"] == "[null]") {
        LOG_INFO(L"%S audio output device is nulled!", getDevName(type));
        port->device = 0;
        return handle;
      } else {
        try {
          (*jData)["device"].get_to(jdname);
          dname = jdname.c_str();
        } catch (const json::exception& e) {
          LOG_ERR(L"Invalid audio device name: %S", e.what());
          dname = NULL;
        }
      }

      for (int i = 0; i < port->channelsNum; i++) {
        port->volume[i] = AudioOut::VOLUME_0DB;
      }
    }

    if ((port->device = SDL_OpenAudioDevice(dname, 0, &fmt, NULL, 0)) == 0) {
      LOG_ERR(L"%S audio device initialization failed: %S", getDevName(type), SDL_GetError());
      return Err::AudioOut::INVALID_PARAM;
    }

    LOG_INFO(L"%S audio device %S opened for user #%d", getDevName(type), dname, userId);
    port->mixedAudio.resize(port->sampleSize * port->samplesNum * port->channelsNum);
    return handle;
  }

  return Err::AudioOut::PORT_FULL;
}

EXPORT SYSV_ABI int32_t sceAudioOutClose(int32_t handle) {
  LOG_USE_MODULE(libSceAudioOut);
  LOG_TRACE(L"%S", __FUNCTION__);
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);
  return pimpl->portsOut.UnacquirePort(handle);
}

EXPORT SYSV_ABI int32_t sceAudioOutOutput(int32_t handle, const void* ptr) {
  auto pimpl = getData();

  // boost::unique_lock const lock(pimpl->mutexInt);
  return writeOut(pimpl, handle, ptr);
}

EXPORT SYSV_ABI int32_t sceAudioOutSetVolume(int32_t handle, int32_t flag, int32_t* vol) {
  LOG_USE_MODULE(libSceAudioOut);
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);

  if (auto port = pimpl->portsOut.GetPort(handle)) {
    for (int i = 0; i < port->channelsNum; i++, flag >>= 1u) {
      bool const bit = flag & 0x1u;
      if (!bit) continue;

      int srcIndex = i;
      if (port->format == SceAudioOutParamFormat::FLOAT_8CH_STD || port->format == SceAudioOutParamFormat::S16_8CH_STD) {
        switch (i) {
          case 4: srcIndex = 6; break;
          case 5: srcIndex = 7; break;
          case 6: srcIndex = 4; break;
          case 7: srcIndex = 5; break;
          default:;
        }
      }
      port->volume[i] = vol[srcIndex];
      LOG_DEBUG(L"port.volume[%d] volume[%d] (%d)", i, srcIndex, vol[srcIndex]);
    }
    return Ok;
  }

  return Err::AudioOut::NOT_OPENED;
}

EXPORT SYSV_ABI int32_t sceAudioOutOutputs(SceAudioOutOutputParam* param, uint32_t num) {
  auto pimpl = getData();

  // boost::unique_lock const lock(pimpl->mutexInt); // dont block, causes audio artifacts
  for (uint32_t i = 0; i < num; i++) {
    if (auto err = writeOut(pimpl, param[i].handle, param[i].ptr); err != 0) return err;
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetLastOutputTime(int32_t handle, uint64_t* outputTime) {
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);

  if (auto port = pimpl->portsOut.GetPort(handle)) {
    *outputTime = port->lastOutputTime;
    return Ok;
  }

  return Err::AudioOut::NOT_OPENED;
}

EXPORT SYSV_ABI int32_t sceAudioOutSetMixLevelPadSpk(int32_t handle, int32_t mixLevel) {
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);

  if (auto port = pimpl->portsOut.GetPort(handle)) {
    if (port->type != SceAudioOutPortType::PADSPK) return Err::AudioOut::INVALID_PORT_TYPE;
    // todo ???
    return Ok;
  }

  return Err::AudioOut::NOT_OPENED;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetPortState(int32_t handle, SceAudioOutPortState* state) {
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);

  if (auto port = pimpl->portsOut.GetPort(handle)) {
    state->channel = port->channelsNum;
    state->volume  = 127;
    state->output  = (uint16_t)SceAudioOutStateOutput::CONNECTED_PRIMARY;
    return Ok;
  }

  return Err::AudioOut::NOT_OPENED;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetSystemState(SceAudioOutSystemState* state) {
  state->loudness = -70.0f;

  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutSetSystemDebugState(SceAudioOutSystemDebugStateElement elem, SceAudioOutSystemDebugStateParam* param) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutMasteringTerm() {
  return Ok;
}
}
