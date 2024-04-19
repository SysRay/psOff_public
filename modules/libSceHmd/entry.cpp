#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceHmd);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceHmd";

EXPORT SYSV_ABI int32_t sceHmdInitialize(const SceHmdInitializeParam* param) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdInitialize315(const SceHmdInitializeParam* param) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdReprojectionInitialize(SceHmdReprojectionResourceInfo* resource,
	                                                   SceHmdReprojectionReprojectionType type,
	                                                   void* option) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI size_t sceHmdReprojectionQueryOnionBuffSize() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI uint32_t sceHmdReprojectionQueryOnionBuffAlign() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI size_t sceHmdReprojectionQueryGarlicBuffSize() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI uint32_t sceHmdReprojectionQueryGarlicBuffAlign() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdReprojectionSetUserEventStart(void* eq, int id) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdReprojectionSetUserEventEnd(void* eq, int id) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdGetDeviceInformation(SceHmdDeviceInformation* info) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdGetDeviceInformationByHandle(int32_t handle,
                                                           SceHmdDeviceInformation* info) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceHmdReprojectionStop() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceHmdReprojectionUnsetDisplayBuffers() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceHmdReprojectionFinalize() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI int sceHmdReprojectionClearUserEventStart() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceHmdReprojectionSetDisplayBuffers(int32_t videoOutHandle,
                                                        int32_t index0, 
                                                        int32_t index1,
                                                        void* option) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceHmdReprojectionSetOutputMinColor(float r,
                                                        float g,
                                                        float b) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdOpen(int32_t userId, 
                                   int32_t _type, 
                                   int32_t index, 
                                   const SceHmdOpenParam* pParam) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdGetFieldOfView(int32_t handle,
                                             SceHmdFieldOfView* fieldOfView) {
  LOG_USE_MODULE(libSceHmd);
   if (fieldOfView == nullptr) {
    return Err::ERROR_PARAMETER_NULL;
   }

   fieldOfView->tanOut    = 1.20743;
   fieldOfView->tanIn     = 1.181346;
   fieldOfView->tanTop    = 1.262872;
   fieldOfView->tanBottom = 1.262872;

  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdGet2DEyeOffset(int32_t handle,
                                             SceHmdEyeOffset* leftEyeOffset,
                                             SceHmdEyeOffset* rightEyeOffset) {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceHmdTerminate() {
  LOG_USE_MODULE(libSceHmd);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}