#pragma once

#include <cstdint>

struct SceAudioOutMasteringStatesHeader {
  uint32_t statesId;
};

struct SceAudioOutMasteringStatesDescriptor {
  uint32_t id;
  uint32_t size;
};

struct SceAudioOutMasteringCompressorStates {
  SceAudioOutMasteringStatesDescriptor desc;
  uint32_t                             reserved[2];
  float                                inputRms[3][8];
  float                                compressionCoeff[3][8];
};

struct SceAudioOutMasteringLimiterStates {
  SceAudioOutMasteringStatesDescriptor desc;
  uint32_t                             reserved[2];
  float                                inputPeak[8];
  float                                outputPeak[8];
  float                                gainPeak[8];
};

struct SceAudioOutMasteringStates {
  SceAudioOutMasteringStatesHeader     statesHeader;
  uint32_t                             reserved[3];
  SceAudioOutMasteringCompressorStates compressorStates;
  SceAudioOutMasteringLimiterStates    limiterStates;
};

struct SceAudioOutMasteringParamsHeader {
  uint32_t paramsId;
};
