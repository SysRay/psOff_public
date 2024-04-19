#define __APICALL_EXTERN
#include "networking.h"
#undef __APICALL_EXTERN
#include "config_emu.h"
#include "states/online.h"
#include "states/offline.h"

class NetInitializer {
  INetworking* m_itf;

public:
  NetInitializer() {
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);
    bool state = false;

    if (getJsonParam(jData, "netEnabled", state) && state == true) {
      static OnlineNet ion;
      m_itf = &ion;
    } else {
      static OfflineNet oon;
      m_itf = &oon;
    }
  }

  INetworking& getClass() {
    return (*m_itf);
  }
};

INetworking& accessNetworking() {
  static NetInitializer ni;
  return ni.getClass();
}
