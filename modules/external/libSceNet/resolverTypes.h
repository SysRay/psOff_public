#pragma once

#include "modules/external/libSceNet/types.h"
#include "types.h"

union SceNetResolverAddrUn {
  SceNetInAddr_t addr;

  char reserved[16];
};

struct SceNetResolverAddr {
  SceNetResolverAddrUn un;

  int af;
  int reserved[3];
};

struct SceNetResolverInfo {
  SceNetResolverAddr addrs[SCE_NET_RESOLVER_MULTIPLE_RECORDS_MAX];

  int records;
  int dns4records;
  int reserved1;
  int reserved[13];
};