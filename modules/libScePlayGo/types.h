#pragma once
#include "codes.h"

enum class ScePlayGoLocusValue : uint32_t {
  NotDownloaded = 0,
  LocalSlow     = 2,
  LocalFast     = 3,
};

enum class ScePlayGoInstallSpeedValue : uint32_t {
  Suspended = 0,
  Trickle   = 1,
  Full      = 2,
};

typedef int32_t  ScePlayGoHandle;
typedef uint16_t ScePlayGoChunkId;
typedef int8_t   ScePlayGoLocus;
typedef int32_t  ScePlayGoInstallSpeed;
typedef int64_t  ScePlayGoEta;
typedef uint64_t ScePlayGoLanguageMask;

struct ScePlayGoInitParams {
  const void* bufAddr;
  uint32_t    bufSize;
  uint32_t    reserved;
};

struct ScePlayGoToDo {
  ScePlayGoChunkId chunkId;
  ScePlayGoLocus   locus;
  int8_t           reserved;
};

struct ScePlayGoProgress {
  uint64_t progressSize;
  uint64_t totalSize;
};
