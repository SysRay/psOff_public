#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"

struct SceNgs2RenderBufferInfo {
  void*    bufferPtr;
  size_t   bufferSize;
  uint32_t waveType;
  uint32_t numChannels;
};

struct SceWaveformFormat;
struct SceSystemOption;

struct SceNgs2ContextBufferInfo {
  void*  hostBuffer;
  size_t hostBufferSize;
};

struct SceNgs2BufferAllocator {
  int32_t (*allocHandler)(SceNgs2ContextBufferInfo*);
  int32_t (*freeHandler)(SceNgs2ContextBufferInfo*);
  void* userData;
};
struct SceNgs2Handle;
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
