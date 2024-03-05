#include "common.h"
#include "core/kernel/eventqueue_types.h"
#include "core/videoout/videoout.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceVideoOut);

namespace {
void event_trigger_func(Kernel::EventQueue::KernelEqueueEvent* event, void* trigger_data) {
  event->triggered = true;
  event->event.fflags++;
  event->event.data = reinterpret_cast<intptr_t>(trigger_data);
}

void event_reset_func(Kernel::EventQueue::KernelEqueueEvent* event) {
  event->triggered    = false;
  event->event.fflags = 0;
  event->event.data   = 0;
}

void event_delete_func(Kernel::EventQueue::IKernelEqueue_t eq, Kernel::EventQueue::KernelEqueueEvent* event) {
  accessVideoOut().removeEvent(event->event.fflags, eq, event->event.ident);
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceVideoOut";

EXPORT SYSV_ABI int32_t sceVideoOutOpen(int32_t userId, int32_t type, int32_t index, const void* param) {
  return accessVideoOut().open(userId);
}

EXPORT SYSV_ABI int32_t sceVideoOutClose(int32_t handle) {
  accessVideoOut().close(handle);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutRegisterBuffers(int32_t handle, int32_t startIndex, void* const* addresses, int32_t bufferNum,
                                                   const SceVideoOutBufferAttribute* attribute) {
  [[unlikely]] if (addresses == nullptr) { return Err::VIDEO_OUT_ERROR_INVALID_ADDRESS; }

  [[unlikely]] if (attribute == nullptr) { return Err::VIDEO_OUT_ERROR_INVALID_OPTION; }

  [[unlikely]] if (startIndex < 0 || startIndex > 15 || bufferNum < 1 || bufferNum > 16 || startIndex + bufferNum > 15) {
    return Err::VIDEO_OUT_ERROR_INVALID_VALUE;
  }

  return accessVideoOut().registerBuffers(handle, startIndex, addresses, bufferNum, attribute);
}

EXPORT SYSV_ABI int32_t sceVideoOutRegisterStereoBuffers(int32_t handle, int32_t startIndex, const SceVideoOutStereoBuffers* buffers, int32_t length,
                                                         const SceVideoOutBufferAttribute* attribute) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutUnregisterBuffers(int32_t handle, int32_t attributeIndex) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutSubmitChangeBufferAttribute(int32_t handle, int32_t index, const SceVideoOutBufferAttribute* attribute) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutSetFlipRate(int32_t handle, int32_t rate) {
  accessVideoOut().setFliprate(handle, rate);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutSetWindowModeMargins(int32_t handle, int top, int bottom) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutSubmitFlip(int32_t handle, int32_t bufferIndex, uint32_t flipMode, int64_t flipArg) {
  if (bufferIndex < 0 || bufferIndex > 15) {
    return Err::VIDEO_OUT_ERROR_INVALID_INDEX;
  }
  accessVideoOut().submitFlip(handle, bufferIndex, flipArg);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetFlipStatus(int32_t handle, SceVideoOutFlipStatus* status) {
  accessVideoOut().getFlipStatus(handle, status);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetVblankStatus(int32_t handle, SceVideoOutVblankStatus* status) {
  accessVideoOut().getVBlankStatus(handle, status);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutIsFlipPending(int32_t handle) {
  return accessVideoOut().getPendingFlips(handle);
  ;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetResolutionStatus(int32_t handle, SceVideoOutResolutionStatus* status) {
  accessVideoOut().getResolution(handle, status);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutAddFlipEvent(Kernel::EventQueue::IKernelEqueue_t eq, int32_t handle, void* udata) {
  [[unlikely]] if (eq == nullptr) { return Err::VIDEO_OUT_ERROR_INVALID_EVENT_QUEUE; }

  Kernel::EventQueue::KernelEqueueEvent const event = {.triggered = false,
                                                       .event =
                                                           {
                                                               .ident  = VIDEO_OUT_EVENT_FLIP,
                                                               .filter = Kernel::EventQueue::KERNEL_EVFILT_VIDEO_OUT,
                                                               .flags  = Kernel::EventQueue::EventFlags::EV_CLEAR,
                                                               .fflags = (uint32_t)handle,
                                                               .data   = 0,
                                                               .udata  = udata,

                                                           },
                                                       .filter {
                                                           .data              = nullptr,
                                                           .trigger_func      = event_trigger_func,
                                                           .reset_func        = event_reset_func,
                                                           .delete_event_func = event_delete_func,
                                                       }};

  return accessVideoOut().addEvent(handle, event, eq);
}

EXPORT SYSV_ABI int32_t sceVideoOutAddVblankEvent(Kernel::EventQueue::IKernelEqueue_t eq, int32_t handle, void* udata) {
  [[unlikely]] if (eq == nullptr) { return Err::VIDEO_OUT_ERROR_INVALID_EVENT_QUEUE; }

  Kernel::EventQueue::KernelEqueueEvent const event = {.triggered = false,
                                                       .event =
                                                           {
                                                               .ident  = VIDEO_OUT_EVENT_VBLANK,
                                                               .filter = Kernel::EventQueue::KERNEL_EVFILT_VIDEO_OUT,
                                                               .flags  = Kernel::EventQueue::EventFlags::EV_CLEAR,
                                                               .fflags = (uint32_t)handle,
                                                               .data   = 0,
                                                               .udata  = udata,

                                                           },
                                                       .filter {
                                                           .data              = nullptr,
                                                           .trigger_func      = event_trigger_func,
                                                           .reset_func        = event_reset_func,
                                                           .delete_event_func = event_delete_func,
                                                       }};

  return accessVideoOut().addEvent(handle, event, eq);
}

EXPORT SYSV_ABI int32_t sceVideoOutDeleteFlipEvent(Kernel::EventQueue::IKernelEqueue_t eq, int32_t handle) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutDeleteVblankEvent(Kernel::EventQueue::IKernelEqueue_t eq, int32_t handle) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutAddPreVblankStartEvent(Kernel::EventQueue::IKernelEqueue_t eq, int32_t handle, void* udata) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutDeletePreVblankStartEvent(Kernel::EventQueue::IKernelEqueue_t eq, int32_t handle) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetEventId(Kernel::EventQueue::KernelEvent_t ev) {
  return (*ev).ident;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetEventData(Kernel::EventQueue::KernelEvent_t ev, int64_t* data) {
  *data = (*ev).data;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetEventCount(Kernel::EventQueue::KernelEvent_t ev) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutWaitVblank(int32_t handle) {
  LOG_USE_MODULE(libSceVideoOut);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceVideoOutSetBufferAttribute(SceVideoOutBufferAttribute* attribute, uint32_t pixelFormat, uint32_t tilingMode, uint32_t aspectRatio,
                                                   uint32_t width, uint32_t height, uint32_t pitchInPixel) {
  accessVideoOut().getBufferAttribute(attribute, pixelFormat, tilingMode, aspectRatio, width, height, pitchInPixel);
}

EXPORT SYSV_ABI int32_t sceVideoOutColorSettingsSetGamma_(SceVideoOutColorSettings* p, float gamma, uint32_t sizeOfSettings) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutAdjustColor_(int32_t handle, SceVideoOutColorSettings* pSettings, uint32_t sizeOfSettings) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorEnable(int32_t handle, int32_t index, const void* address) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorDisable(int32_t handle, int32_t index) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorSetHotSpot(int32_t handle, int32_t index, uint32_t hotX, uint32_t hotY) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorSetPosition(int32_t handle, int32_t index, uint32_t posX, uint32_t posY) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorSetPositionStereo(int32_t handle, int32_t index, uint32_t posX, uint32_t posY, uint32_t offset) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorSet2xMagnify(int32_t handle, int32_t index, uint32_t enable) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorSetImageAddress(int32_t handle, int32_t index, const void* address) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutCursorIsUpdatePending(int32_t handle, int32_t index, SceVideoOutCursorPendingType type) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVideoOutGetDeviceCapabilityInfo_(int32_t handle, SceVideoOutDeviceCapabilityInfo* pInfo, size_t sizeOfInfo) {
  pInfo->capability = 0;
  return Ok;
}

EXPORT SYSV_ABI void sceVideoOutModeSetAny_(SceVideoOutMode* pMode, uint32_t sizeOfMode) {}

EXPORT SYSV_ABI void sceVideoOutConfigureOptionsInitialize_(SceVideoOutConfigureOptions* pOptions, uint32_t sizeOfOptions) {}

EXPORT SYSV_ABI int32_t sceVideoOutConfigureOutputMode_(int32_t handle, uint32_t reserved, SceVideoOutMode const* pMode,
                                                        SceVideoOutConfigureOptions const* pOptions, uint32_t sizeOfMode, uint32_t sizeOfOptions) {
  return Ok;
}
}