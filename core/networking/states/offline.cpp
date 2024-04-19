#include "offline.h"
#include "logging.h"
#include "modules/libSceNetCtl/codes.h"

LOG_DEFINE_MODULE(OfflineNetworkingCore)

int32_t OfflineNet::netCtlGetInfo(int32_t code, SceNetCtlInfo* info) {
  LOG_USE_MODULE(OfflineNetworkingCore);
  LOG_TRACE(L"netCtlGetInfo(%d, %p)", code, info);
  ::memset(info, 0, sizeof(SceNetCtlInfo));
  return Err::NetCtl::NOT_CONNECTED; // todo simulate available network connection?
}

int32_t OfflineNet::resolverCreate() {
  return 0;
}
