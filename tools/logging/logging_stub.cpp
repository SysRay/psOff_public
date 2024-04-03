#define __APICALL_EXTERN
#include "logging.h"
#undef __APICALL_EXTERN

#include <stdarg.h>
#include <string>

namespace __Log {
void* __registerLoggingModule(std::wstring_view name) {
  return nullptr;
}

void __log(eTrace_Level level, void* hmodule, unsigned short i_wLine, const char* i_pFile, const char* i_pFunction, const wchar_t* i_pFormat, ...) {}

void flush() {}

void flushExceptional() {}

uint8_t isIgnored(void* module, eTrace_Level level) {
  return 0;
}
} // namespace __Log