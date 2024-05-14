#pragma once

#include "types.h"

class Reader {
  void* m_pimpl;
  bool  m_isInit       = false;
  bool  m_isCompressed = false;

  SceNgs2Handle_voice* voice;

  SceNgs2SamplerVoiceState m_state;

  bool getAudioUncompressed(SceNgs2RenderBufferInfo*, uint32_t numOutSamples);
  bool getAudioCompressed(SceNgs2RenderBufferInfo*);

  public:
  Reader(SceNgs2Handle_voice* voice);
  ~Reader();

  bool init(SceNgs2SamplerVoiceWaveformBlocksParam const* param);

  bool getAudio(SceNgs2RenderBufferInfo*, uint32_t numOutSamples);

  void setNewData(void const* start, void const* end);

  void getState(SceNgs2SamplerVoiceState* state) const {
    *state            = m_state;
    state->voiceState = voice->state;
  }
};
