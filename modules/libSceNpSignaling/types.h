#pragma once
#include "../libSceNet/types.h"
#include "codes.h"

struct SceNpOnlineId {
  char data[SCE_NP_ONLINEID_MAX_LENGTH];
  char term;
  char dummy[3];
};

struct SceNpId {
  SceNpOnlineId handle;
  unsigned char opt[8];
  unsigned char reserved[8];
};

typedef void SYSV_ABI (*SceNpSignalingHandler)(uint32_t ctxId, uint32_t subjectId, int event, int errorCode, void* arg);

struct SceNpSignalingNetInfo {
  size_t       size;
  SceNetInAddr local_addr;
  SceNetInAddr mapped_addr;
  int          nat_status;
};
