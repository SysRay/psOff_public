#pragma once
#include "codes.h"

typedef unsigned int SceAjmContextId;
typedef unsigned int SceAjmCodecType;
typedef unsigned int SceAjmInstanceId;
typedef unsigned int SceAjmBatchId;

struct SceAjmSidebandStreamResult {
  struct {
      int iSizeConsumed;
      int iSizeProduced;
      unsigned int uiTotalDecodedSamples;
  } sStream;
};

struct SceAjmBatchError {
  int   iErrorCode;
  int   align1;
  void* pJobAddress;
  int   uiCommandOffset;
  int   align2;
  void* pJobOriginRa;
};