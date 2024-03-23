#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"

enum SceNgs2WaveFormType : uint32_t {
  NONE = 0,

  PCM_I8 = 0x10,
  PCM_U8 = 0x11,

  PCM_I16LITTLE = 0x12,
  PCM_I16BIG    = 0x13,

  PCM_I24LITTLE = 0x14,
  PCM_I24BIG    = 0x15,

  PCM_I32LITTLE = 0x16,
  PCM_I32BIG    = 0x17,

  PCM_F32LITTLE = 0x18,
  PCM_F32BIG    = 0x19,

  PCM_F64LITTLE = 0x1A,
  PCM_F64BIG    = 0x1B,

  VAG = 0x1C,

  MAX_TYPES

  /* More? */
};

enum SceNgs2ChannelsCount : uint32_t {
  CH_1_0 = 1,
  CH_2_0 = 2,
  CH_5_1 = 6,
  CH_7_1 = 8,
};

struct SceNgs2ContextBufferInfo {
  void*     hostBuffer;
  size_t    hostBufferSize;
  uintptr_t pad[5];
  void*     userData;
};

struct SceNgs2BufferAllocator {
  int32_t (*allocHandler)(SceNgs2ContextBufferInfo*);
  int32_t (*freeHandler)(SceNgs2ContextBufferInfo*);
  void* userData;
};

struct SceNgs2Handle {
  SceNgs2ContextBufferInfo cbi;
};

struct SceNgs2RenderBufferInfo {
  void*                bufferPtr;
  size_t               bufferSize;
  SceNgs2WaveFormType  waveType;
  SceNgs2ChannelsCount channelsCount;
};

struct SceNgs2WaveformInfo {
  SceNgs2WaveFormType  type;
  SceNgs2ChannelsCount channelsCount;
  uint32_t             sampleRate;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
};

struct SceNgs2WaveformBlock {
  uint32_t offset;
  uint32_t size;
  uint32_t  : 32;
  uint32_t  : 32;
  uint32_t  : 32;
  uintptr_t : 64;
};

struct SceNgs2WaveformFormat {
  SceNgs2WaveformInfo info;
  uint32_t            offset;
  uint32_t            size;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t samplesCount;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t : 32;
  SceNgs2WaveformBlock block[4];
};

struct SceNgs2SystemOption {
  size_t   size;
  char     name[16];
  uint32_t flags;
  uint32_t : 32;
  uint32_t : 32;
  uint32_t sampleRate;
  int8_t   pad[24];
};

struct SceNgs2GeomListenerWork;
struct SceNgs2GeomListenerParam;

typedef int (*SceWaveformUserFunc)(uintptr_t ud, uint32_t off, void* data, size_t size);

struct SceNgs2GeomVector {
  float x, y, z;
};

struct SceNgs2GeomSourceParam {
  SceNgs2GeomVector pos;
  SceNgs2GeomVector vel;
  SceNgs2GeomVector dir;
  /* todo */
};
