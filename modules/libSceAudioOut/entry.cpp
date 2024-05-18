#include "common.h"
#include "config_emu.h"
#include "logging.h"
#include "types.h"

#include <SDL2/SDL.h>
#include <array>
#include <chrono>
#include <core/videoout/videoout.h>
#include <mutex>

LOG_DEFINE_MODULE(libSceAudioOut);

namespace {
static bool audioInited = false;

struct PortOut {
  bool                   open           = false;
  int                    userId         = 0;
  SceAudioOutPortType    type           = SceAudioOutPortType::MAIN;
  uint8_t                sampleSize     = 0;
  uint32_t               sampleRate     = 0;
  uint32_t               samplesNum     = 0;
  uint32_t               queued         = 0;
  SceAudioOutParamFormat format         = SceAudioOutParamFormat::FLOAT_MONO;
  uint64_t               lastOutputTime = 0;
  int                    channelsNum    = 0;
  int                    volume[8]      = {};
  SDL_AudioDeviceID      device         = 0;
  SDL_AudioFormat        sdlFormat      = AUDIO_F32;
  float                  volumeModifier = 0.5f;
  std::vector<uint8_t>   mixedAudio;
  std::string            deviceName;

  SDL_AudioSpec audioSpec;
};

class PortsOut {
  private:
  std::array<PortOut, AudioOut::PORT_OUT_MAX> m_ports = {};

  static inline uint8_t rangeStart(uint16_t range) { return (range >> 8) & 0xFF; }

  static inline uint8_t rangeEnd(uint16_t range) { return range & 0xFF; }

  public:
  PortsOut() {
    auto handler = [](SDL_Event* event, void* ud) {
      if (event->adevice.iscapture) return; // Ignoring capture device events
      PortsOut* self = (PortsOut*)ud;
      switch (event->type) {
        case SDL_AUDIODEVICEADDED: self->AlertConnect(event->adevice.which); break;

        case SDL_AUDIODEVICEREMOVED: self->AlertDisconnect(event->adevice.which); break;
      }
    };

    accessVideoOut().SDLEventReg(SDL_AUDIODEVICEADDED, handler, this);
    accessVideoOut().SDLEventReg(SDL_AUDIODEVICEREMOVED, handler, this);
  }

  void AlertConnect(int devIndex) {
    LOG_USE_MODULE(libSceAudioOut);
    auto devName = SDL_GetAudioDeviceName(devIndex, 0);

    for (int i = 0; i < m_ports.size(); i++) {
      auto& port = m_ports[i];
      if (!port.open || port.device != 0 || port.deviceName.length() == 0) continue;
      /**
       * SDL appends to reconnected device's name "(<number>)" string.
       * So we have to politely ignore it.
       */
      if (port.deviceName.compare(0, std::string::npos, devName, 0, port.deviceName.length()) != 0) continue;

      if ((port.device = SDL_OpenAudioDevice(devName, 0, &port.audioSpec, NULL, 0)) == 0) {
        LOG_ERR(L"Failed to reopen %S audio device: %S", devName, SDL_GetError());
        return;
      }

      LOG_INFO(L"Welcome back, %S!", devName);
      return;
    }
  }

  void AlertDisconnect(int devIndex) {
    LOG_USE_MODULE(libSceAudioOut);
    for (int i = 0; i < m_ports.size(); i++) {
      auto& port = m_ports[i];
      if (!port.open || (port.device != devIndex)) continue;
      SDL_CloseAudioDevice(port.device);
      port.device = 0;
      LOG_ERR(L"Oh no! %S got disconnected", port.deviceName.c_str());
      return;
    }
  }

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

  Pimpl(): portsOut() {}
};

Pimpl* getData() {
  static Pimpl obj;
  return &obj;
}

void syncPort(PortOut* port) {
  if (port == nullptr || !port->open) return;
  const uint32_t bytesize_1ch = port->samplesNum * port->sampleSize;
  const uint32_t bytesize     = bytesize_1ch * port->channelsNum;

  if (port->device == 0) {
    float duration = bytesize_1ch / float(port->sampleRate * port->sampleSize);
    SDL_Delay(int(duration * 1000)); // Pretending that we playing something
    return;
  }

  while (port->queued > (bytesize * 2)) {
    SDL_Delay(0);
    port->queued = SDL_GetQueuedAudioSize(port->device);
  }
}

void clearQueue(PortOut* port) {
  if (port == nullptr || !port->open || port->device == 0) return;
  SDL_ClearQueuedAudio(port->device);
  port->queued = 0;
}

int writeOut(PortOut* port, const void* ptr, bool sync = true) {
  if (port == nullptr) return Err::AudioOut::NOT_OPENED;
  if (ptr == nullptr) return 0;

  using namespace std::chrono;
  port->lastOutputTime = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

  const uint32_t bytesize_1ch = port->samplesNum * port->sampleSize;

  if (port->device == 0) {
    LOG_USE_MODULE(libSceAudioOut);
    LOG_TRACE(L"Nulled audio port, playing nothing (port=%p)", port);
    if (sync) syncPort(port);
    return port->samplesNum;
  }

  const int maxVolume = SDL_MIX_MAXVOLUME * port->volumeModifier;
  auto&     mixed     = port->mixedAudio;
  std::fill(mixed.begin(), mixed.end(), 0);

  for (int i = 0; i < port->audioSpec.channels; i++) {
    auto ch_offset = i * bytesize_1ch;
    SDL_MixAudioFormat(mixed.data() + ch_offset, ((const uint8_t*)ptr) + ch_offset, port->sdlFormat, bytesize_1ch,
                       maxVolume * ((float)port->volume[i] / AudioOut::VOLUME_0DB));
  }

  if (SDL_GetAudioDeviceStatus(port->device) != SDL_AUDIO_PLAYING) SDL_PauseAudioDevice(port->device, 0);

  int ret = SDL_QueueAudio(port->device, mixed.data(), port->audioSpec.size);
  if (ret == 0) port->queued = SDL_GetQueuedAudioSize(port->device);
  if (sync) syncPort(port);

  return ret == 0 ? port->samplesNum : Err::AudioOut::TRANS_EVENT;
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

EXPORT SYSV_ABI int32_t sceAudioOutOpen(int32_t userId, SceAudioOutPortType type, int32_t index, uint32_t numSamples, uint32_t sampleRate, uint32_t param) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
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
    port->samplesNum = numSamples;
    port->sampleRate = sampleRate;
    port->format     = SceAudioOutParamFormat(param & 0x0000007F);

    if ((param & 0x000F0000) != 0) {
      LOG_ERR(L"todo handle attributes");
    }

    switch (port->format) {
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

    std::string jdname; // Device name from JSON
    const char* dname;  // Actual device name that will be passed to SDL
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::AUDIO);

    try {
      (*jData)["volume"].get_to(port->volumeModifier);
    } catch (const json::exception& e) {
      LOG_ERR(L"Invalid audio volume setting: %S", e.what());
      port->volumeModifier = 0.5f;
    }

    SDL_AudioSpec audioSpec = {
        .freq     = static_cast<int>(sampleRate),
        .format   = port->sdlFormat,
        .channels = static_cast<uint8_t>(port->channelsNum),
        .samples  = static_cast<uint16_t>(port->samplesNum),
        .callback = nullptr,
        .userdata = nullptr,
    };

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
        port->volume[i] = AudioOut::MIXLEVEL_PADSPK_0DB;
      }
    } else {
      if ((*jData)["device"] == "[default]") {
        if (SDL_GetDefaultAudioInfo((char**)&dname, &port->audioSpec, 0) != 0) {
          LOG_ERR(L"Failed to get the default audio device, port #%d is nulled! (%S)", handle, SDL_GetError());
          port->device = 0;
          return handle;
        }
      } else if ((*jData)["device"] == "[null]") {
        LOG_INFO(L"%S audio output device is nulled!", getDevName(type));
        port->device = 0;
        return handle;
      } else {
        try {
          (*jData)["device"].get_to(jdname);
          dname = jdname.c_str();
        } catch (const json::exception& e) {
          LOG_ERR(L"Invalid audio device name: %S, falling back to default", e.what());
          dname = NULL;
          if (SDL_GetDefaultAudioInfo((char**)&dname, &port->audioSpec, 0) != 0) {
            LOG_ERR(L"Falling back to default device failed, port #%d is nulled! (%S)", handle, SDL_GetError());
            port->device = 0;
            return handle;
          }
        }
      }

      for (int i = 0; i < port->channelsNum; i++) {
        port->volume[i] = AudioOut::VOLUME_0DB;
      }
    }

    if ((port->device = SDL_OpenAudioDevice(dname, 0, &audioSpec, &port->audioSpec, 0)) == 0) {
      LOG_ERR(L"Failed to open %S audio device: %S", getDevName(type), SDL_GetError());
      // Since audio connect/disconnect event is implemented now, we can fall through there.
      // The audio device will be opened automatically when available.
    } else {
      LOG_INFO(L"%S audio device %S opened for user #%d", getDevName(type), dname, userId);
    }

    port->mixedAudio.resize(port->audioSpec.size);
    port->deviceName.assign(dname);
    return handle;
  }

  return Err::AudioOut::PORT_FULL;
}

EXPORT SYSV_ABI int32_t sceAudioOutClose(int32_t handle) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  LOG_USE_MODULE(libSceAudioOut);
  LOG_TRACE(L"%S", __FUNCTION__);
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);
  return pimpl->portsOut.UnacquirePort(handle);
}

EXPORT SYSV_ABI int32_t sceAudioOutOutput(int32_t handle, const void* ptr) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  auto pimpl = getData();

  // boost::unique_lock const lock(pimpl->mutexInt);
  return writeOut(pimpl->portsOut.GetPort(handle), ptr);
}

EXPORT SYSV_ABI int32_t sceAudioOutSetVolume(int32_t handle, int32_t flag, int32_t* vol) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
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
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  if (param == nullptr) return Err::AudioOut::INVALID_POINTER;
  if (num == 0) return Err::AudioOut::PORT_FULL;
  auto pimpl = getData();

  // boost::unique_lock const lock(pimpl->mutexInt); // dont block, causes audio artifacts
  PortOut* firstport = nullptr;

  for (uint32_t i = 0; i < num; i++) {
    auto port = pimpl->portsOut.GetPort(param[i].handle);
    if (auto err = writeOut(port, param[i].ptr, false); err <= 0) {
      for (uint32_t j = 0; j < num; j++) {
        clearQueue(pimpl->portsOut.GetPort(param[i].handle));
      }
      return err;
    }
    if (!firstport)
      firstport = port;
    else if (firstport->samplesNum != port->samplesNum)
      return Err::AudioOut::INVALID_SIZE;
  }

  /**
   * Passing the audio from SDL to OS. We should do this thing
   * only once since all the devices playing audio simultaneously.
   */
  syncPort(firstport);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutGetLastOutputTime(int32_t handle, uint64_t* outputTime) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  auto pimpl = getData();

  boost::unique_lock const lock(pimpl->mutexInt);

  if (auto port = pimpl->portsOut.GetPort(handle)) {
    *outputTime = port->lastOutputTime;
    return Ok;
  }

  return Err::AudioOut::NOT_OPENED;
}

EXPORT SYSV_ABI int32_t sceAudioOutSetMixLevelPadSpk(int32_t handle, int32_t mixLevel) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
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
  if (!audioInited) return Err::AudioOut::NOT_INIT;
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
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  state->loudness = -70.0f;

  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutSetSystemDebugState(SceAudioOutSystemDebugStateElement elem, SceAudioOutSystemDebugStateParam* param) {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioOutMasteringTerm() {
  if (!audioInited) return Err::AudioOut::NOT_INIT;
  return Ok;
}
}
