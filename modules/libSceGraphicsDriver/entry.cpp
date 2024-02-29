#include "common.h"
#include "core/kernel/eventqueue_types.h"
#include "types.h"

#include <graphics.h>
#include <logging.h>
#include <pm4_custom.h>
#include <timer.h>
#include <videoOut.h>

LOG_DEFINE_MODULE(libSceGraphicsDriver);

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
  accessVideoOut().getGraphics()->removeEvent(eq, event->event.ident);
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceGraphicsDriver";

int SYSV_ABI sceGnmSetVsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* vs_regs, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::R_VS);
  cmdOut[1] = shader_modifier;
  memcpy(&cmdOut[2], vs_regs, size - 1);
  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmUpdateVsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* vs_regs, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::R_VS_UPDATE);
  cmdOut[1] = shader_modifier;
  memcpy(&cmdOut[2], vs_regs, size - 1);
  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmSetPsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  if (ps_regs == nullptr) {
    cmdOut[0] = Pm4::create(size, Pm4::R_PS_EMBEDDED);
    cmdOut[1] = 0;
  } else {
    cmdOut[0] = Pm4::create(size, Pm4::R_PS);
    memcpy(&cmdOut[1], ps_regs, 8 + size);
  }

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmSetPsShader350(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  if (ps_regs == nullptr) {
    cmdOut[0] = Pm4::create(size, Pm4::R_PS_EMBEDDED);
    cmdOut[1] = 0;
  } else {
    cmdOut[0] = Pm4::create(size, Pm4::R_PS);
    memcpy(&cmdOut[1], ps_regs, 8 + size);
  }

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmUpdatePsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::R_PS_UPDATE);
  memcpy(&cmdOut[1], ps_regs, 8 + size);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmUpdatePsShader350(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  cmdOut[0] = Pm4::create(size, Pm4::R_PS_UPDATE);
  memcpy(&cmdOut[1], ps_regs, 8 + size);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmSetCsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* cs_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  cmdOut[0] = Pm4::create(size, Pm4::R_CS);
  cmdOut[1] = 0;
  memcpy(&cmdOut[2], cs_regs, size);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmSetCsShaderWithModifier(uint32_t* cmdOut, uint64_t size, const uint32_t* cs_regs, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  cmdOut[0] = Pm4::create(size, Pm4::R_CS);
  cmdOut[1] = shader_modifier;
  memcpy(&cmdOut[2], cs_regs, size);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmSetEmbeddedVsShader(uint32_t* cmdOut, uint64_t size, uint32_t id, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::R_VS_EMBEDDED);
  cmdOut[1] = shader_modifier;
  cmdOut[2] = id;

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmDrawIndex(uint32_t* cmdOut, uint64_t size, uint32_t index_count, const void* index_addr, uint32_t flags, uint32_t type) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::R_DRAW_INDEX);
  cmdOut[1] = index_count;
  cmdOut[2] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(index_addr) & 0xffffffffu);
  cmdOut[3] = static_cast<uint32_t>((reinterpret_cast<uint64_t>(index_addr) >> 32u) & 0xffffffffu);
  cmdOut[4] = flags;
  cmdOut[5] = type;

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmDrawIndexAuto(uint32_t* cmdOut, uint64_t size, uint32_t index_count, uint32_t flags) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::R_DRAW_INDEX_AUTO);
  cmdOut[1] = index_count;
  cmdOut[2] = flags;

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int32_t SYSV_ABI sceGnmValidateDrawCommandBuffers(uint32_t count, void* dcbGpuAddrs[], uint32_t* dcbSizesInBytes, void* ccbGpuAddrs[],
                                                  uint32_t* ccbSizesInBytes) {
#if DEBUG
  LOG_USE_MODULE(libSceGraphicsDriver);
  for (uint32_t n = 0; n < count; ++n) {
    if (dcbGpuAddrs != nullptr) LOG_DEBUG(L"Validate DCB[%d] 0x%08llx(0x%u)", n, (uint64_t)dcbGpuAddrs[n], dcbSizesInBytes[n]);
    if (ccbGpuAddrs != nullptr) LOG_DEBUG(L"Validate CCB[%d] 0x%08llx(0x%u)", n, (uint64_t)ccbGpuAddrs[n], ccbSizesInBytes[n]);
  }
#endif
  return Err::VALIDATION_NOT_ENABLED;
}

int SYSV_ABI sceGnmSubmitCommandBuffers(uint32_t count, void** dcb_gpu_addrs, const uint32_t* dcb_sizes_in_bytes, void** ccb_gpu_addrs,
                                        const uint32_t* ccb_sizes_in_bytes) {
  accessVideoOut().getGraphics()->submitCmdBuffer(count, (uint32_t const**)dcb_gpu_addrs, dcb_sizes_in_bytes, (uint32_t const**)ccb_gpu_addrs,
                                                  ccb_sizes_in_bytes, 0, 0, 0, 0, false);
  return Ok;
}

int SYSV_ABI sceGnmSubmitCommandBuffersForWorkload(uint64_t workload, uint32_t count, void** dcb_gpu_addrs, const uint32_t* dcb_sizes_in_bytes,
                                                   void** ccb_gpu_addrs, const uint32_t* ccb_sizes_in_bytes) {
  accessVideoOut().getGraphics()->submitCmdBuffer(count, (uint32_t const**)dcb_gpu_addrs, dcb_sizes_in_bytes, (uint32_t const**)ccb_gpu_addrs,
                                                  ccb_sizes_in_bytes, 0, 0, 0, 0, false);
  return Ok;
}

int SYSV_ABI sceGnmSubmitAndFlipCommandBuffers(uint32_t count, void** dcb_gpu_addrs, const uint32_t* dcb_sizes_in_bytes, void** ccb_gpu_addrs,
                                               const uint32_t* ccb_sizes_in_bytes, int handle, int index, int flip_mode, int64_t flip_arg) {
  accessVideoOut().getGraphics()->submitCmdBuffer(count, (uint32_t const**)dcb_gpu_addrs, dcb_sizes_in_bytes, (uint32_t const**)ccb_gpu_addrs,
                                                  ccb_sizes_in_bytes, handle, index, flip_mode, flip_arg, true);
  return Ok;
}

int SYSV_ABI sceGnmSubmitAndFlipCommandBuffersForWorkload(uint64_t workload, uint32_t count, void** dcb_gpu_addrs, const uint32_t* dcb_sizes_in_bytes,
                                                          void** ccb_gpu_addrs, const uint32_t* ccb_sizes_in_bytes, int handle, int index, int flip_mode,
                                                          int64_t flip_arg) {
  accessVideoOut().getGraphics()->submitCmdBuffer(count, (uint32_t const**)dcb_gpu_addrs, dcb_sizes_in_bytes, (uint32_t const**)ccb_gpu_addrs,
                                                  ccb_sizes_in_bytes, handle, index, flip_mode, flip_arg, true);
  return Ok;
}

int SYSV_ABI sceGnmSubmitDone() {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);
  accessVideoOut().getGraphics()->waitSubmitDone();
  return Ok;
}

int SYSV_ABI sceGnmAreSubmitsAllowed() {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  return accessVideoOut().getGraphics()->isRunning();
}

void SYSV_ABI sceGnmFlushGarlic() {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);
}

int SYSV_ABI sceGnmSetVgtControl(uint32_t* cmdOut, uint64_t size, uint32_t primGroupSizeMinusOne, uint32_t partialVsWaveMode, uint32_t wdSwitchOnlyOnEopMode) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  if (size == 3 && primGroupSizeMinusOne < 0x100 && ((wdSwitchOnlyOnEopMode | partialVsWaveMode) < 2)) {
    cmdOut[0] = Pm4::create(size, Pm4::R_VGT_CONTROL);
    cmdOut[1] = 0x2aa;
    cmdOut[2] = ((partialVsWaveMode & 1) << 16) or (primGroupSizeMinusOne & 0xffff);

    accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
    return Ok;
  }
  return -1;
}

int SYSV_ABI sceGnmResetVgtControl(uint32_t* cmdOut, int32_t param) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  if (param == 3) {
    cmdOut[0] = Pm4::create(3, Pm4::R_VGT_CONTROL);
    cmdOut[1] = 0x2aa;
    cmdOut[2] = 0xff;

    accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + 3);
    return Ok;
  }
  return -1;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::R_DRAW_RESET);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut[2]) + 2);
  return 2;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState175(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::R_DRAW_RESET);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + 2);
  return 2;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState200(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::R_DRAW_RESET);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + 2);
  return 2;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState350(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::R_DRAW_RESET);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + 2);
  return 2;
}

uint32_t SYSV_ABI sceGnmDispatchInitDefaultHardwareState(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::R_DISPATCH_RESET);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + 2);
  return 2;
}

int SYSV_ABI sceGnmInsertWaitFlipDone(uint32_t* cmdOut, uint64_t size, uint32_t video_out_handle, uint32_t display_buffer_index) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::R_WAIT_FLIP_DONE);
  cmdOut[1] = video_out_handle;
  cmdOut[2] = display_buffer_index;

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmDispatchDirect(uint32_t* cmdOut, uint64_t size, uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z, uint32_t mode) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::R_DISPATCH_DIRECT);
  cmdOut[1] = thread_group_x;
  cmdOut[2] = thread_group_y;
  cmdOut[3] = thread_group_z;
  cmdOut[4] = mode;

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

uint32_t SYSV_ABI sceGnmMapComputeQueue(uint32_t pipe_id, uint32_t queue_id, uint32_t* ring_addr, uint32_t ring_size_dw, uint32_t* read_ptr_addr) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  // todo compute queue (8 commandprocessors with 8 compute rings (total 64))
  uint32_t const v = pipe_id * 8 + queue_id;
  *read_ptr_addr   = 0;
  return v + 1;
}

int SYSV_ABI sceGnmComputeWaitOnAddress(uint32_t* cmdOut, uint64_t size, uint32_t* gpu_addr, uint32_t mask, uint32_t func, uint32_t ref) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::R_DISPATCH_WAIT_MEM);
  cmdOut[1] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(gpu_addr) & 0xffffffffu);
  cmdOut[2] = static_cast<uint32_t>((reinterpret_cast<uint64_t>(gpu_addr) >> 32u) & 0xffffffffu);
  cmdOut[3] = mask;
  cmdOut[4] = func;
  cmdOut[5] = ref;
  return Ok;
}

void SYSV_ABI sceGnmDingDong(uint32_t ring_id, uint32_t offset_dw) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  // todo dingdong (cumpute ring)
}

void SYSV_ABI sceGnmUnmapComputeQueue(uint32_t id) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  // todo unmap (abort?)
}

int SYSV_ABI sceGnmInsertPushMarker(uint32_t* cmdOut, uint64_t size, const char* str) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  auto const len = strlen(str) + 1;
  cmdOut[0]      = Pm4::create(size, Pm4::R_PUSH_MARKER);
  // memcpy(cmdOut + 1, str, len);

  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

int SYSV_ABI sceGnmInsertPopMarker(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::R_POP_MARKER);
  accessVideoOut().getGraphics()->updateCmdBuffer(((uint64_t)cmdOut) + size);
  return Ok;
}

uint64_t SYSV_ABI sceGnmGetGpuCoreClockFrequency() {
  return 0x800000000;
}

int SYSV_ABI sceGnmIsUserPaEnabled() {
  return 0;
}

void* SYSV_ABI sceGnmGetTheTessellationFactorRingBufferBaseAddress() {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  return 0; // Maps/allocates it afterwards anyway
}

// ####   Only used for tracing ########
// #####################################

int SYSV_ABI sceGnmRegisterOwner(uint32_t* owner_handle, const char* name) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  // Objects::GpuMemoryRegisterOwner(owner_handle, name);
  return Ok;
}

int SYSV_ABI sceGnmRegisterResource(uint32_t* resource_handle, uint32_t owner_handle, const void* memory, size_t size, const char* name, uint32_t type,
                                    uint64_t user_data) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);
  uint32_t rhandle = 0;
  // Objects::GpuMemoryRegisterResource(&rhandle, owner_handle, memory, size, name, type, user_data);
  if (resource_handle != nullptr) {
    *resource_handle = rhandle;
  }

  return Err::FAILURE;
}

int SYSV_ABI sceGnmUnregisterAllResourcesForOwner(uint32_t owner_handle) {
  return Ok;
}

int SYSV_ABI sceGnmUnregisterOwnerAndResources(uint32_t owner_handle) {
  return Ok;
}

int SYSV_ABI sceGnmUnregisterResource(uint32_t resource_handle) {
  return Ok;
}

// - tracing
// #####################################

int SYSV_ABI sceGnmAddEqEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id, void* udata) {
  Kernel::EventQueue::KernelEqueueEvent event = {.triggered = false,
                                                 .event =
                                                     {
                                                         .ident  = id,
                                                         .filter = Kernel::EventQueue::KERNEL_EVFILT_GRAPHICS,
                                                         .flags  = Kernel::EventQueue::EventFlags::EV_CLEAR,
                                                         .fflags = 0,
                                                         .data   = 0,
                                                         .udata  = udata,

                                                     },
                                                 .filter {
                                                     .data              = nullptr,
                                                     .trigger_func      = event_trigger_func,
                                                     .reset_func        = event_reset_func,
                                                     .delete_event_func = event_delete_func,
                                                 }};

  return accessVideoOut().getGraphics()->addEvent(event, eq);
}

int SYSV_ABI sceGnmDeleteEqEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id) {
  accessVideoOut().getGraphics()->removeEvent(eq, id);
  return Ok;
}

void SYSV_ABI sceGnmInsertSetMarker(const char* debugString) {}

void SYSV_ABI sceGnmInsertSetColorMarker(const char* debugString, uint32_t argbColor) {}

SceWorkloadStatus SYSV_ABI sceGnmCreateWorkloadStream(const char* name, SceWorkloadStream* stream) {
  static int32_t count = 0;
  *stream              = ++count;
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"+workloadstream %S -> id:%d", name, *stream);
  return SceWorkloadStatus::StatusOk;
}

SceWorkloadStatus SYSV_ABI sceGnmBeginWorkload(SceWorkloadStream stream, uint64_t* workload) {
  static int32_t count = 0;
  *workload            = ++count;
  return SceWorkloadStatus::StatusOk;
}

SceWorkloadStatus SYSV_ABI sceGnmEndWorkload(uint64_t workload) {
  return SceWorkloadStatus::StatusOk;
}

SceWorkloadStatus SYSV_ABI sceGnmDestroyWorkloadStream(SceWorkloadStream workloadStream) {
  return SceWorkloadStatus::StatusOk;
}
}
