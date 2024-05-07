#pragma once

struct SceNgs2Handle_voice;
struct SceNgs2SamplerVoiceWaveformBlocksParam;

class Reader {
  void* m_pimpl;
  bool  m_isInit = false;

  public:
  Reader();
  ~Reader();

  bool init(SceNgs2SamplerVoiceWaveformBlocksParam const* param);

  bool getAudio(SceNgs2Handle_voice* handle, void* buffer, size_t bufferSize);
};