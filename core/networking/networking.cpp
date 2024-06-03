#define __APICALL_EXTERN
#include "networking.h"
#undef __APICALL_EXTERN
#include "config_emu.h"
#include "logging.h"
#include "states/offline.h"
#include "states/online.h"

LOG_DEFINE_MODULE(NetworkingCore);

static thread_local int32_t g_net_errno = 0;

INetworking::INetworking() {
  LOG_USE_MODULE(NetworkingCore);
  WSADATA data = {};
  if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) LOG_CRIT(L"WSAStartup failed: %d", getLastError());
}

INetworking::~INetworking() {
  LOG_USE_MODULE(NetworkingCore);
  if (WSACleanup() == SOCKET_ERROR) LOG_CRIT(L"WSACleanup failed: %d", getLastError());
}

int32_t INetworking::getLastError() {
  auto win_err = (uint32_t)GetLastError();
  if (win_err == WSANOTINITIALISED) return Err::Net::ERROR_ENOTINIT;
  return (0x80000000 | (0x041 << 16) | (0x0100 | (win_err - 10000)));
}

int32_t* INetworking::getErrnoPtr() {
  return &g_net_errno;
}

class NetInitializer {
  INetworking* m_itf;

  public:
  NetInitializer() {
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);
    bool state         = false;

    if (getJsonParam(jData, "netEnabled", state) && state == true) {
      static OnlineNet ion;
      m_itf = &ion;
    } else {
      static OfflineNet oon;
      m_itf = &oon;
    }
  }

  INetworking& getClass() { return (*m_itf); }
};

INetworking& accessNetworking() {
  static NetInitializer ni;
  return ni.getClass();
}
