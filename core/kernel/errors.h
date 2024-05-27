#pragma once
#include "modules_include/common.h"
#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL int* getError_pthread();
__APICALL void setError_pthread(int);

/**
 * @brief Takes Kernel errors from gerErr(), converts and saves it
 *
 * @param result from gerErr()
 * @return int32_t
 */
static int32_t POSIX_CALL(int32_t result) {
  if (result >= 0) return result;

  int res = result - (int32_t)0x80020000;
  setError_pthread(res);
  return res;
}

static int32_t POSIX_SET(ErrCode error) {
  setError_pthread((int32_t)error);
  return -1;
}

#undef __APICALL
