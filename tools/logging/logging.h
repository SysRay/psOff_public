#pragma once

#ifdef LOGGING_DUMMY
#include "logging_dummy.h"
#else

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

#include <string>

namespace __Log {
constexpr wchar_t const* __APPNAME = L"psOff";

__APICALL void* __registerLoggingModule(std::wstring&& name); // For internal usage

enum class eTrace_Level : uint8_t {
  trace = 0,
  debug,
  info,
  warn,
  err,
  crit,
};

__APICALL void __log(eTrace_Level level, void* hmodule, unsigned short i_wLine, const char* i_pFile, const char* i_pFunction, const wchar_t* i_pFormat, ...);

/**
 * @brief Call at end to flush messages
 *
 */
__APICALL void deinit();

__APICALL uint8_t isIgnored(void* module, eTrace_Level level);
} // namespace __Log

/**
 * @brief Call at start of file or in class
 *
 */
#define LOG_DEFINE_MODULE(name)                                                                                                                                \
  namespace {                                                                                                                                                  \
  void* __TRACE_MODULE__DEFINED__##name() {                                                                                                                    \
    static void* obj = __Log::__registerLoggingModule(L#name);                                                                                                 \
    return obj;                                                                                                                                                \
  }                                                                                                                                                            \
  }

/**
 * @brief Call before logging
 *
 */
#define LOG_USE_MODULE(name) [[maybe_unused]] void* __TRACE_MODULE = __TRACE_MODULE__DEFINED__##name()
#define LOG_GET_MODULE(name) __TRACE_MODULE__DEFINED__##name()

// Logging
#define LOG_TRACE(...)       __Log::__log(__Log::eTrace_Level::trace, __TRACE_MODULE, (unsigned short)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...)       __Log::__log(__Log::eTrace_Level::debug, __TRACE_MODULE, (unsigned short)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)        __Log::__log(__Log::eTrace_Level::info, __TRACE_MODULE, (unsigned short)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(...)        __Log::__log(__Log::eTrace_Level::warn, __TRACE_MODULE, (unsigned short)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERR(...)         __Log::__log(__Log::eTrace_Level::err, __TRACE_MODULE, (unsigned short)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_CRIT(...)        __Log::__log(__Log::eTrace_Level::crit, __TRACE_MODULE, (unsigned short)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
// -

#undef __APICALL
#endif