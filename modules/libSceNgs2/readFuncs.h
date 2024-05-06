#pragma once
#include "types.h"

using funcReadBuf_t = int (*)(void*, uint8_t*, int);
using funcSeekBuf_t = int64_t (*)(void*, int64_t, int);

struct userData_user {
  SceWaveformUserFunc func;
  uintptr_t           userData;
  size_t              curOffset;
};

struct userData_inerBuffer {
  const void* ptr;
  size_t      size;
  size_t      curOffset;
};

int     readFunc_linearBuffer(void* userData_, uint8_t* buf, int size);
int64_t seekFunc_linearBuffer(void* userData_, int64_t offset, int whence);
int     readFunc_file(void* userData_, uint8_t* buf, int size);
int64_t seekFunc_file(void* userData_, int64_t offset, int whence);
int     readFunc_user(void* userData_, uint8_t* buf, int size);
int64_t seekFunc_user(void* userData_, int64_t offset, int whence);

int32_t parseWave(funcReadBuf_t readFunc, funcSeekBuf_t seekFunc, void* userData, SceNgs2WaveformFormat* wf);