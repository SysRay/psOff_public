#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libScePngDec);

extern "C" {

EXPORT const char* MODULE_NAME = "libScePngDec";

EXPORT SYSV_ABI int32_t scePngDecCreate(const ScePngDecCreateParam* param, void* ptr, uint32_t size, ScePngDecHandle* handle) {
  LOG_USE_MODULE(libScePngDec);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecDecode(ScePngDecHandle handle, const ScePngDecDecodeParam* param, ScePngDecImageInfo* imageInfo) {
  LOG_USE_MODULE(libScePngDec);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecDelete(ScePngDecHandle handle) {
  LOG_USE_MODULE(libScePngDec);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecParseHeader(const ScePngDecParseParam* param, ScePngDecImageInfo* ii) {
  LOG_USE_MODULE(libScePngDec);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecQueryMemorySize(const ScePngDecCreateParam* param) {
  LOG_USE_MODULE(libScePngDec);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
