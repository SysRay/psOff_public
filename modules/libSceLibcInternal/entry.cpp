#include "common.h"
#include "core/imports/imports_runtime.h"
#include "core/unwinding/unwind.h"
#include "logging.h"
#include "types.h"

#include <boost/thread.hpp>
#include <cstdlib>
#include <vector>
LOG_DEFINE_MODULE(libSceLibcInternal);

namespace {} // namespace

// ### OBJECTS
extern "C" {
EXPORT auto     __NID(_Stdout)              = stdout;
EXPORT uint32_t __NID(Need_sceLibcInternal) = 1;
}
// - ### OBJECTS

extern "C" {

EXPORT const char* MODULE_NAME = "libSceLibcInternal";

/**
 * @brief std::ios_base::Init::Init
 *
 */
EXPORT SYSV_ABI void __NID(_ZNSt8ios_base4InitC1Ev)() {}

/**
 * @brief std::_Winit::_Winit()
 *
 */
EXPORT SYSV_ABI void __NID(_ZNSt6_WinitC1Ev)() {}

EXPORT SYSV_ABI void __NID(_init_env)() {
  LOG_USE_MODULE(libSceLibcInternal);
  LOG_TRACE(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI int __NID(vprintf)(const char* str, va_list args) {
  LOG_USE_MODULE(libSceLibcInternal);

  std::string format(str);
  LOG_DEBUG(std::wstring(format.begin(), format.end()).c_str(), args);

  return Ok;
}

EXPORT SYSV_ABI int __NID(snprintf)(char* s, size_t n, const char* format, va_list args) {
  return std::snprintf(s, n, format, args);
}

EXPORT SYSV_ABI int __NID(fflush)(FILE* stream) {
  return std::fflush(stream);
}

EXPORT SYSV_ABI void* __NID(memset)(void* s, int c, size_t n) {
  return std::memset(s, c, n);
}

EXPORT SYSV_ABI void* __NID(memcpy)(void* d, void* s, size_t n) {
  return std::memcpy(d, s, n);
}

EXPORT SYSV_ABI void* __NID(memmove)(void* dst, void const* src, size_t n) {
  return std::memmove(dst, src, n);
}

EXPORT SYSV_ABI errno_t __NID(memcpy_s)(void* dst, size_t dsize, const void* src, size_t count) {
  return ::memcpy_s(dst, dsize, src, count);
}

EXPORT SYSV_ABI errno_t __NID(memcmp)(void* mem1, const void* mem2, size_t size) {
  return ::memcmp(mem1, mem2, size);
}

EXPORT SYSV_ABI size_t __NID(strlen)(const char* str) {
  return ::strlen(str);
}

EXPORT SYSV_ABI size_t __NID(strnlen)(const char* str, size_t max) {
  return ::strnlen(str, max);
}

EXPORT SYSV_ABI errno_t __NID(strnlen_s)(const char* str, size_t max) {
  return ::strnlen_s(str, max);
}

EXPORT SYSV_ABI char* __NID(strncat)(char* dst, const char* src, size_t size) {
  return ::strncat(dst, src, size);
}

EXPORT SYSV_ABI errno_t __NID(strncat_s)(char* dst, size_t dstsz, const char* src, size_t count) {
  return ::strncat_s(dst, dstsz, src, count);
}

EXPORT SYSV_ABI char* __NID(strstr)(char* haystack, const char* needle) {
  return ::strstr(haystack, needle);
}

EXPORT SYSV_ABI int __NID(strncmp)(const char* str1, const char* str2, size_t size) {
  return ::strncmp(str1, str2, size);
}

EXPORT SYSV_ABI char* __NID(strncpy)(char* dst, char* src, size_t len) {
  return ::strncpy(dst, src, len);
}

EXPORT SYSV_ABI errno_t __NID(strncpy_s)(char* dst, size_t dstsz, const char* src, size_t count) {
  return ::strncpy_s(dst, dstsz, src, count);
}

EXPORT SYSV_ABI int __NID(strcmp)(const char* str1, const char* str2) {
  return ::strcmp(str1, str2);
}

EXPORT SYSV_ABI char* __NID(strcpy)(char* dst, const char* src) {
  return ::strcpy(dst, src);
}

EXPORT SYSV_ABI int __NID(strcoll)(const char* str1, const char* str2) {
  return ::strcoll(str1, str2);
}

EXPORT SYSV_ABI size_t __NID(strcspn)(const char* str, const char* charset) {
  return ::strcspn(str, charset);
}

EXPORT SYSV_ABI errno_t __NID(strcpy_s)(char* dst, size_t size, const char* src) {
  return ::strcpy_s(dst, size, src);
}

EXPORT SYSV_ABI const char* __NID(strchr)(const char* str, int val) {
  return ::strchr(str, val);
}

EXPORT SYSV_ABI const char* __NID(strrchr)(const char* str, int val) {
  return ::strrchr(str, val);
}

EXPORT SYSV_ABI errno_t __NID(strcat_s)(char* dst, size_t sz, const char* src) {
  return ::strcat_s(dst, sz, src);
}

EXPORT SYSV_ABI const char* __NID(strcat)(char* dst, const char* src) {
  return ::strcat(dst, src);
}

EXPORT SYSV_ABI const char* __NID(strtok)(char* str, const char* delim) {
  return ::strtok(str, delim);
}

EXPORT SYSV_ABI const char* __NID(strtok_r)(char* str, const char* delim, char** ctx) {
  return ::strtok_s(str, delim, ctx);
}

EXPORT SYSV_ABI int __NID(strcasecmp)(const char* dst, const char* src) {
  return ::_stricmp(dst, src);
}

EXPORT SYSV_ABI char* __NID(strdup)(const char* src) {
  return ::_strdup(src);
}

EXPORT SYSV_ABI int __NID(strncasecmp)(const char* dst, const char* src, size_t num) {
  return ::_strnicmp(dst, src, num);
}

EXPORT SYSV_ABI long __NID(strtol)(const char* str, char** end, int rad) {
  return ::strtol(str, end, rad);
}

EXPORT SYSV_ABI long double __NID(strtold)(const char* str, char** end) {
  return ::strtold(str, end);
}

EXPORT SYSV_ABI long long __NID(strtoll)(const char* str, char** end, int rad) {
  return ::strtoll(str, end, rad);
}

EXPORT SYSV_ABI unsigned long long __NID(strtoull)(const char* str, char** end, int rad) {
  return ::strtoull(str, end, rad);
}

EXPORT SYSV_ABI unsigned long __NID(strtoul)(const char* str, char** end, int rad) {
  return ::strtoul(str, end, rad);
}

EXPORT SYSV_ABI int __NID(_Stoul)(const char* str, char** endptr, int base) {
  return ::_Stoulx(str, endptr, base, nullptr);
}

EXPORT SYSV_ABI unsigned long __NID(strxfrm)(char* dst, const char* src, size_t max) {
  return ::strxfrm(dst, src, max);
}

EXPORT SYSV_ABI float __NID(expf)(float val) {
  return std::expf(val);
}

EXPORT SYSV_ABI double __NID(exp)(double val) {
  return std::exp(val);
}

EXPORT SYSV_ABI float __NID(sinf)(float val) {
  return std::sinf(val);
}

EXPORT SYSV_ABI double __NID(sin)(double val) {
  return std::sin(val);
}

EXPORT SYSV_ABI float __NID(_FSin)(float val, int p) {
  return p ? std::cosf(val) : std::sinf(val);
}

EXPORT SYSV_ABI float __NID(_FSinh)(float a, float b) {
  return ::sinhf(a) * b;
}

EXPORT SYSV_ABI float __NID(cosf)(float val) {
  return std::cosf(val);
}

EXPORT SYSV_ABI double __NID(cos)(double val) {
  return std::cos(val);
}

EXPORT SYSV_ABI float __NID(sqrtf)(float val) {
  return std::sqrtf(val);
}

EXPORT SYSV_ABI double __NID(sqrt)(double val) {
  return std::sqrt(val);
}

EXPORT SYSV_ABI float __NID(tanf)(float val) {
  return std::tanf(val);
}

EXPORT SYSV_ABI double __NID(tan)(double val) {
  return std::tan(val);
}

EXPORT SYSV_ABI float __NID(atan2f)(float x, float y) {
  return std::atan2f(x, y);
}

EXPORT SYSV_ABI double __NID(atan2)(double x, double y) {
  return std::atan2(x, y);
}

EXPORT SYSV_ABI float __NID(asinf)(float val) {
  return std::asinf(val);
}

EXPORT SYSV_ABI double __NID(asin)(double val) {
  return std::asin(val);
}

EXPORT SYSV_ABI float __NID(powf)(float val, float p) {
  return std::powf(val, p);
}

EXPORT SYSV_ABI double __NID(pow)(double val, double p) {
  return std::pow(val, p);
}

EXPORT SYSV_ABI float __NID(exp2f)(float val) {
  return std::exp2f(val);
}

EXPORT SYSV_ABI double __NID(exp2)(double val) {
  return std::exp2(val);
}

EXPORT SYSV_ABI int __NID(setjmp)(unwinding_jmp_buf* jb) {
  return 0;
}

EXPORT SYSV_ABI int __NID(gmtime_s)(tm* tm, const time_t* time) {
  return ::gmtime_s(tm, time);
}

static boost::recursive_mutex _guard_mtx;

struct GuardObject {
  bool initHasRun;
  bool inUse;
};

EXPORT SYSV_ABI int __NID(__cxa_guard_acquire)(GuardObject* go) {
  LOG_USE_MODULE(libSceLibcInternal);
  if (go->initHasRun) return 0;

  _guard_mtx.lock();

  if (go->initHasRun) {
    _guard_mtx.unlock();
    return 0;
  }

  if (go->inUse) LOG_CRIT(L"initializer for function local static variable called enclosing function");

  go->inUse = true;
  return 1;
}

EXPORT SYSV_ABI int __NID(__cxa_guard_release)(GuardObject* go) {
  go->initHasRun = true;
  _guard_mtx.unlock();
  return 0;
}

EXPORT SYSV_ABI int __NID(__cxa_guard_abort)(GuardObject* go) {
  _guard_mtx.unlock();
  go->inUse = false;
  return 0;
}

EXPORT SYSV_ABI int __NID(__cxa_atexit)(void (*func)(void*), void* arg, int moduleId) {
  LOG_USE_MODULE(libSceLibcInternal);
  LOG_TRACE(L"%S", __FUNCTION__);

  accessRuntimeExport()->cxa_add_atexit(
      CxaDestructor {
          .destructor_func   = func,
          .destructor_object = arg,
      },
      moduleId);
  return 0;
}

EXPORT SYSV_ABI void __NID(__cxa_finalize)(int moduleId) {
  LOG_USE_MODULE(libSceLibcInternal);
  LOG_DEBUG(L"%S", __FUNCTION__);

  accessRuntimeExport()->cxa_finalize(moduleId);
}

typedef void SYSV_ABI (*newhandler_func)(void* mem);

/**
 * @brief operator.new(sz)
 *
 */
EXPORT SYSV_ABI void* __NID(_Znwm)(size_t sz) {
  return new char[sz];
}

/**
 * @brief operator.new(sz) nothrow
 *
 */
EXPORT SYSV_ABI void* __NID(_ZnwmRKSt9nothrow_t)(size_t sz, void* tag) {
  return new (std::nothrow) char[sz];
}

/**
 * @brief operator.new[](sz)
 *
 */
EXPORT SYSV_ABI void* __NID(_Znam)(size_t sz) {
  return new char[sz];
}

/**
 * @brief operator.new[](sz) nothrow
 *
 */
EXPORT SYSV_ABI void* __NID(_ZnamRKSt9nothrow_t)(size_t sz, void* tag) {
  return new (std::nothrow) char[sz];
}

/**
 * @brief operator.delete[](ptr)
 *
 */
EXPORT SYSV_ABI void __NID(_ZdaPv)(char* ptr) {
  delete ptr;
}

/**
 * @brief operator.delete[](ptr)
 *
 */
EXPORT SYSV_ABI void __NID(_ZdaPvm)(char* ptr, size_t sz) {
  delete ptr;
}

/**
 * @brief operator.delete[](ptr, sz) nothrow
 *
 */
EXPORT SYSV_ABI void __NID(_ZdaPvmRKSt9nothrow_t)(char* ptr, size_t sz, void* tag) {
  if (ptr != nullptr) delete ptr;
}

/**
 * @brief operator.delete[](ptr) nothrow
 *
 */
EXPORT SYSV_ABI void __NID(_ZdaPvRKSt9nothrow_t)(char* ptr, void* tag) {
  if (ptr != nullptr) delete ptr;
}

/**
 * @brief operator.delete(ptr, sz) nothrow
 *
 */
EXPORT SYSV_ABI void __NID(_ZdlPvmRKSt9nothrow_t)(char* ptr, size_t sz, void* tag) {
  if (ptr != nullptr) delete ptr;
}

/**
 * @brief operator.delete(ptr) nothrow
 *
 */
EXPORT SYSV_ABI void __NID(_ZdlPvRKSt9nothrow_t)(char* ptr, void* tag) {
  if (ptr != nullptr) delete ptr;
}

/**
 * @brief std::set_new_handler
 *
 */
EXPORT SYSV_ABI newhandler_func __NID(_ZSt15set_new_handlerPFvvE)(newhandler_func nh) {
  LOG_USE_MODULE(libSceLibcInternal);
  LOG_CRIT(L"Unimplemented std::set_new_handler()");
  return nullptr;
}

/**
 * @brief std::get_new_handler
 *
 */
EXPORT SYSV_ABI newhandler_func __NID(_ZSt15get_new_handlerv)() {
  LOG_USE_MODULE(libSceLibcInternal);
  LOG_ERR(L"Unimplemented std::get_new_handler()");
  return nullptr;
}
}
