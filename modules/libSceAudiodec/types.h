#pragma once
#include "codes.h"

struct SceAudiodecAuInfo {
  uint32_t uiSize;
  void*    pAuAddr;
  uint32_t uiAuSize;
};

struct SceAudiodecPcmItem {
  uint32_t uiSize;
  void*    pPcmAddr;
  uint32_t uiPcmSize;
};

struct SceAudiodecCtrl {
  void*               pParam;
  void*               pBsiInfo;
  SceAudiodecAuInfo*  pAuInfo;
  SceAudiodecPcmItem* pPcmItem;
};

struct SceAudiodecCpuResource {
  uint32_t uiSize;
  void*    pWorkMemAddr;
  uint32_t uiWorkMemSize;
};