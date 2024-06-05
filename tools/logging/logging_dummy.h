#pragma once
#include <string>

namespace __Log {

inline void* __registerModule(std::wstring&& name) {
  return nullptr;
}

enum class eTrace_Level : uint8_t {
  trace = 0,
  debug,
  info,
  warn,
  err,
  crit,
};

void __log(bool doExit, const wchar_t* i_pFormat, ...);

/**
 * @brief Call at end to flush messages
 *
 */
constexpr void deinit() {}

/**
 * @brief Register thread (for better logging)
 *
 * @param name
 */

constexpr uint8_t isIgnored(void* module, eTrace_Level level) {
  return true;
}

constexpr void flush() {}
} // namespace __Log

/**
 * @brief Call at start of file or in class
 *
 */
#define LOG_DEFINE_MODULE(name) void* __TRACE_MODULE__DEFINED__##name = nullptr;

/**
 * @brief Call before logging
 *
 */
#define LOG_USE_MODULE(name) [[maybe_unused]] void* __TRACE_MODULE = __TRACE_MODULE__DEFINED__##name
#define LOG_GET_MODULE(name) __TRACE_MODULE__DEFINED__##name

// Logging
#define LOG_TRACE(...) /**/
#define LOG_DEBUG(...) /**/
#define LOG_INFO(...)  /**/
#define LOG_WARN(...)  /**/
#define LOG_ERR(...)   __Log::__log(false, __VA_ARGS__)
#define LOG_CRIT(...)  __Log::__log(true, __VA_ARGS__)
// -
