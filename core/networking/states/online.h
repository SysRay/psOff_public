#pragma once
#include "../networking.h"

// Sigh, Microsoft
// clang-format off
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iphlpapi.h>
// clang-format on

class OnlineNet : public INetworking {
public:

  /* SceNetCtl facility*/

  /**
   * @brief gets information about network parameters
   *
   * @param code
   * @param info
   * @return int32_t
   */
  int32_t netCtlGetInfo(int code, SceNetCtlInfo* info) final;

  /* SceNet facility */

  /* Resolver sub-facility */
  int32_t resolverCreate() final;

  /* Epoll sub-facility */
  // todo

  /* Socket sub-facility */
  // guess what? todo.
};
