#pragma once

struct SceNgs2Handle_voice;
struct SceNgs2SamplerVoiceWaveformBlocksParam;
struct SceNgs2RenderBufferInfo;

class Reader {
  void* m_pimpl;
  bool  m_isInit       = false;
  bool  m_isCompressed = false;

  SceNgs2Handle_voice* voice;

  bool getAudioUncompressed(SceNgs2RenderBufferInfo*);
  bool getAudioCompressed(SceNgs2RenderBufferInfo*);

  public:
  Reader(SceNgs2Handle_voice* voice);
  ~Reader();

  bool init(SceNgs2SamplerVoiceWaveformBlocksParam const* param);

  bool getAudio(SceNgs2RenderBufferInfo*);
};
