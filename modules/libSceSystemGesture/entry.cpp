#include "common.h"
#include "errorcodes.h"
#include "logging.h"

LOG_DEFINE_MODULE(libSceSystemGesture);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSystemGesture";

EXPORT SYSV_ABI int sceSystemGestureInitializePrimitiveTouchRecognizer(void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureOpen(int32_t type, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureCreateTouchRecognizer() {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureAppendTouchRecognizer(int32_t handle, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureUpdatePrimitiveTouchRecognizer() {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureUpdateAllTouchRecognizer(int32_t handle) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureGetTouchEventsCount(int32_t handle, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureClose(int32_t handle, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureFinalizePrimitiveTouchRecognizer(int32_t handle, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureGetTouchEventByIndex(int32_t handle, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceSystemGestureRemoveTouchRecognizer(int32_t handle, void* param) {
  LOG_USE_MODULE(libSceSystemGesture);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}


}
