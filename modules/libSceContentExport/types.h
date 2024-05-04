#pragma once
#include "codes.h"
#include "common.h"

typedef void* SYSV_ABI (*SceContentExportMalloc)(size_t size, void* userData);
typedef void  SYSV_ABI (*SceContentExportFree)(void* ptr, void* userData);

struct SceContentExportParam {
  SceContentExportMalloc mallocFunc;
  SceContentExportFree   freeFunc;
  void*                  userData;
};

struct SceContentExportInitParam2 {
  SceContentExportMalloc mallocfunc;
  SceContentExportFree   freefunc;
  void*                  userdata;
  size_t                 bufsize;
  int64_t                reserved[2];
};
