#pragma once
#include "codes.h"

struct SceAudiodecCpuAuInfo {
  uint32_t uiSize;
	void*    pAuAddr;
	uint32_t uiAuSize;
};

struct SceAudiodecCpuPcmItem {
  uint32_t uiSize;
	void*    pPcmAddr;
	uint32_t uiPcmSize;
};

struct SceAudiodecCpuCtrl {	
  void*                  pParam;
	void*                  pBsiInfo;
	SceAudiodecCpuAuInfo*  pAuInfo;
	SceAudiodecCpuPcmItem* pPcmItem;
};

struct SceAudiodecCpuResource {
  uint32_t uiSize;
	void*    pWorkMemAddr;
	uint32_t uiWorkMemSize;
};