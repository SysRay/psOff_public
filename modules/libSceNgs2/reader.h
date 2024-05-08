#pragma once

struct SceNgs2Handle_voice;
struct SceNgs2SamplerVoiceWaveformBlocksParam;

class Reader {
  void* m_pimpl;
  bool  m_isInit       = false;
  bool  m_isCompressed = false;

  SceNgs2Handle_voice* parent;

  bool getAudioUncompressed(void* buffer, size_t bufferSize);
  bool getAudioCompressed(void* buffer, size_t bufferSize);

  public:
  Reader(SceNgs2Handle_voice* handle);
  ~Reader();

  bool init(SceNgs2SamplerVoiceWaveformBlocksParam const* param);

  bool getAudio(void* buffer, size_t bufferSize);
};