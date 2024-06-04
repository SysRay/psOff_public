#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceZlib);

namespace {
static bool     g_bInitialized = false;
static uint64_t g_reqId        = 0;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceZlib";

EXPORT SYSV_ABI int32_t sceZlibInitialize(const void* buffer, size_t len) {
  if (g_bInitialized) return Err::Zlib::ALREADY_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_ERR(L"todo %S", __FUNCTION__);
  g_bInitialized = true;
  g_reqId        = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibFinalize() {
  if (!g_bInitialized) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_ERR(L"todo %S", __FUNCTION__);
  g_bInitialized = false;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibInflate(const void* src, uint32_t srcLen, void* dst, uint32_t dstLen, uint64_t* reqId) {
  if (!g_bInitialized) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *reqId = ++g_reqId;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceZlibWaitForDone(uint64_t* reqId, uint32_t* timeout) {
  if (!g_bInitialized) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Err::Zlib::TIMEDOUT;
}

EXPORT SYSV_ABI int32_t sceZlibGetResult(uint64_t reqId, uint32_t* dstLen, int* status) {
  if (!g_bInitialized) return Err::Zlib::NOT_INITIALIZED;
  LOG_USE_MODULE(libSceZlib);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  *status = -1;
  return Err::Zlib::NOT_FOUND;
}
}
