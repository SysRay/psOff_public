#include "logging_dummy.h"

#include <stdarg.h>

void __Log::__log(bool doExit, const wchar_t* i_pFormat, ...) {
  va_list args = nullptr;
  va_start(args, i_pFormat);

  vwprintf(i_pFormat, args);
  printf("\n");

  va_end(args);

  if (doExit) exit(1);
}