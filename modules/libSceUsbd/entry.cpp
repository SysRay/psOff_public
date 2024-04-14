#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceUsbd);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceUsbd";

EXPORT SYSV_ABI int sceUsbdInit() {
  LOG_USE_MODULE(libSceUsbd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI ssize_t sceUsbdGetDeviceList(SceUsbdDevice ***list) {
  LOG_USE_MODULE(libSceUsbd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceUsbdFreeDeviceList(SceUsbdDevice **list, int unrefDevices) {
  LOG_USE_MODULE(libSceUsbd);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI int32_t sceUsbdHandleEventsTimeout(struct timeval *tv) {
  LOG_USE_MODULE(libSceUsbd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUsbdOpenDeviceWithVidPid(uint16_t vendorId, uint16_t productId) {
  LOG_USE_MODULE(libSceUsbd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}