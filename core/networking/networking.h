#pragma once

#include "modules/libSceNetCtl/types.h"
#include "modules_include/common.h"
#include "utility/utility.h"

class INetworking {
  CLASS_NO_COPY(INetworking);
  CLASS_NO_MOVE(INetworking);

  protected:
  INetworking() {}

  public:
  virtual ~INetworking() = default;

  /* SceNetCtl facility*/

  /**
   * @brief gets information about network parameters
   *
   * @param code
   * @param info
   * @return int32_t
   */
  virtual int32_t netCtlGetInfo(int code, SceNetCtlInfo* info) = 0;

  /* SceNet facility */

  /* Resolver sub-facility */
  virtual int32_t resolverCreate() = 0;

  /* Epoll sub-facility */
  // todo

  /* Socket sub-facility */
  // guess what? todo.
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL INetworking& accessNetworking();
#undef __APICALL
