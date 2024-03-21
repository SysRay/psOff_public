#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"

typedef struct SceNgs2RenderBufferInfo {
  void*    bufferPtr;
  size_t   bufferSize;
  uint32_t waveType;
  uint32_t numChannels;
} SceNgs2RenderBufferInfo;
typedef struct SceWaveformFormat SceWaveformFormat;
typedef struct SceSystemOption   SceSystemOption;

typedef struct SceNgs2ContextBufferInfo {
  void*  hostBuffer;
  size_t hostBufferSize;
} SceNgs2ContextBufferInfo;

typedef struct SceNgs2BufferAllocator {
  int32_t (*allocHandler)(SceNgs2ContextBufferInfo*);
  int32_t (*freeHandler)(SceNgs2ContextBufferInfo*);
  void* userData;
} SceNgs2BufferAllocator;
typedef struct SceNgs2Handle            SceNgs2Handle;
typedef struct SceNgs2GeomListenerWork  SceNgs2GeomListenerWork;
typedef struct SceNgs2GeomListenerParam SceNgs2GeomListenerParam;
typedef int (*SceWaveformUserFunc)(uintptr_t ud, uint32_t off, void* data, size_t size);

typedef struct SceNgs2GeomVector {
  float x, y, z;
} SceNgs2GeomVector;

typedef struct SceNgs2GeomSourceParam {
  SceNgs2GeomVector pos;
  SceNgs2GeomVector vel;
  SceNgs2GeomVector dir;
  /* todo */
} SceNgs2GeomSourceParam;
