#define __APICALL_EXTERN
#include "errors.h"
#undef __APICALL_EXTERN

int* getError_pthread() {
  thread_local int g_errno = 0;
  return &g_errno;
}

void setError_pthread(int error) {
  // todo tracing stacktrace?
  *getError_pthread() = error;
}
