#include "common.h"
#include "core/imports/exports/graphics.h"
#include "core/imports/exports/pm4_custom.h"
#include "core/kernel/eventqueue_types.h"
#include "core/memory/memory.h"
#include "core/timer/timer.h"
#include "core/videoout/videoout.h"
#include "logging.h"
#include "types.h"

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

static memory::_t_hook _hook_setVsShader;

static SYSV_ABI void hook_setVsShader(void* buffer) {
  accessVideoOut().getGraphics()->registerCommandBuffer(buffer);
  ((__stdcall void (*)(void const*))_hook_setVsShader.data.data())(buffer);
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceGraphicsDriver";

int SYSV_ABI sceGnmSetVsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* vs_regs, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);

  // // Nothing to see! hooking caller
  // static bool once = false;

  // if (!once) {
  //   once               = true;
  //   auto const retAddr = (uint64_t)_ReturnAddress();
  //   // find start of caller
  //   std::array     funcStart = {0x55, 0x48, 0x89, 0xe5};
  //   uint8_t const* pCode     = (uint8_t const*)retAddr - 5 - funcStart.size();

  //   uint64_t callAddr = 0;
  //   for (size_t n = 0; n < 200; n++, pCode--) {
  //     if (std::equal(funcStart.data(), funcStart.data() + funcStart.size(), pCode)) {
  //       callAddr = (uint64_t)pCode;
  //       break;
  //     }
  //   }
  //   // -
  //   if (callAddr != 0) {
  //     LOG_INFO(L"Hooked %S", __FUNCTION__);
  //     installHook_long((uintptr_t)hook_setVsShader, callAddr, _hook_setVsShader, 16);
  //   }
  // }
  // // -- hooking caller

  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_VS);
  cmdOut[1] = shader_modifier;
  memcpy(&cmdOut[2], vs_regs, size - 1);

  return Ok;
}

int SYSV_ABI sceGnmUpdateVsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* vs_regs, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_VS_UPDATE);
  cmdOut[1] = shader_modifier;
  memcpy(&cmdOut[2], vs_regs, size - 1);

  return Ok;
}

int SYSV_ABI sceGnmSetPsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  if (ps_regs == nullptr) {
    cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS_EMBEDDED);
    cmdOut[1] = 0;
  } else {
    cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS);
    memcpy(&cmdOut[1], ps_regs, 8 + size);
  }

  return Ok;
}

int SYSV_ABI sceGnmSetPsShader350(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  if (ps_regs == nullptr) {
    cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS_EMBEDDED);
    cmdOut[1] = 0;
  } else {
    cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS);
    memcpy(&cmdOut[1], ps_regs, 8 + size);
  }

  return Ok;
}

int SYSV_ABI sceGnmUpdatePsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS_UPDATE);
  memcpy(&cmdOut[1], ps_regs, 8 + size);

  return Ok;
}

int SYSV_ABI sceGnmUpdatePsShader350(uint32_t* cmdOut, uint64_t size, const uint32_t* ps_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS_UPDATE);
  memcpy(&cmdOut[1], ps_regs, 8 + size);

  return Ok;
}

int SYSV_ABI sceGnmSetCsShader(uint32_t* cmdOut, uint64_t size, const uint32_t* cs_regs) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_CS);
  cmdOut[1] = 0;
  memcpy(&cmdOut[2], cs_regs, size);
  return Ok;
}

int SYSV_ABI sceGnmSetCsShaderWithModifier(uint32_t* cmdOut, uint64_t size, const uint32_t* cs_regs, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);
  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_CS);
  cmdOut[1] = shader_modifier;
  memcpy(&cmdOut[2], cs_regs, size);
  return Ok;
}

int SYSV_ABI sceGnmSetEmbeddedPsShader(uint32_t* cmdOut, uint64_t size, uint32_t id, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_PS_EMBEDDED);
  cmdOut[1] = shader_modifier;
  cmdOut[2] = id;
  return Ok;
}

int SYSV_ABI sceGnmSetEmbeddedVsShader(uint32_t* cmdOut, uint64_t size, uint32_t id, uint32_t shader_modifier) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_VS_EMBEDDED);
  cmdOut[1] = shader_modifier;
  cmdOut[2] = id;
  return Ok;
}

int SYSV_ABI sceGnmDrawIndex(uint32_t* cmdOut, uint64_t size, uint32_t index_count, const void* index_addr, uint32_t flags, uint32_t type) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_DRAW_INDEX);
  cmdOut[1] = index_count;
  cmdOut[2] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(index_addr) & 0xffffffffu);
  cmdOut[3] = static_cast<uint32_t>((reinterpret_cast<uint64_t>(index_addr) >> 32u) & 0xffffffffu);
  cmdOut[4] = flags;
  cmdOut[5] = type;
  return Ok;
}

int SYSV_ABI sceGnmDrawIndexOffset(uint32_t* cmdOut, uint64_t size, uint32_t index_offset, uint32_t index_count, uint32_t flags) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_DRAW_INDEX_OFFSET);
  cmdOut[1] = index_count;
  cmdOut[2] = index_offset;
  cmdOut[3] = flags;
  cmdOut[4] = 0;
  cmdOut[5] = 0;
  cmdOut[6] = 0;
  return Ok;
}

int SYSV_ABI sceGnmDrawIndexAuto(uint32_t* cmdOut, uint64_t size, uint32_t index_count, uint32_t flags) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S 0x%08llx", __FUNCTION__, (uint64_t)cmdOut);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_DRAW_INDEX_AUTO);
  cmdOut[1] = index_count;
  cmdOut[2] = flags;

  return Ok;
}

int32_t SYSV_ABI sceGnmValidateDrawCommandBuffers(uint32_t count, void* dcbGpuAddrs[], uint32_t* dcbSizesInBytes, void* ccbGpuAddrs[],
                                                  uint32_t* ccbSizesInBytes) {
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
    cmdOut[0] = Pm4::create(size, Pm4::Custom::R_VGT_CONTROL);
    cmdOut[1] = 0x2aa;
    cmdOut[2] = ((partialVsWaveMode & 1) << 16) or (primGroupSizeMinusOne & 0xffff);
    return Ok;
  }
  return -1;
}

int SYSV_ABI sceGnmResetVgtControl(uint32_t* cmdOut, int32_t param) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  if (param == 3) {
    cmdOut[0] = Pm4::create(3, Pm4::Custom::R_VGT_CONTROL);
    cmdOut[1] = 0x2aa;
    cmdOut[2] = 0xff;

    return Ok;
  }
  return -1;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::Custom::R_DRAW_RESET);
  return 2;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState175(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::Custom::R_DRAW_RESET);

  return 2;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState200(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::Custom::R_DRAW_RESET);

  return 2;
}

uint32_t SYSV_ABI sceGnmDrawInitDefaultHardwareState350(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::Custom::R_DRAW_RESET);
  return 2;
}

uint32_t SYSV_ABI sceGnmDispatchInitDefaultHardwareState(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_DEBUG(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(2, Pm4::Custom::R_DISPATCH_RESET);

  return 2;
}

int SYSV_ABI sceGnmInsertWaitFlipDone(uint32_t* cmdOut, uint64_t size, uint32_t video_out_handle, uint32_t display_buffer_index) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_WAIT_FLIP_DONE);
  cmdOut[1] = video_out_handle;
  cmdOut[2] = display_buffer_index;

  return Ok;
}

int SYSV_ABI sceGnmDispatchDirect(uint32_t* cmdOut, uint64_t size, uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z, uint32_t mode) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_DISPATCH_DIRECT);
  cmdOut[1] = thread_group_x;
  cmdOut[2] = thread_group_y;
  cmdOut[3] = thread_group_z;
  cmdOut[4] = mode;

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

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_DISPATCH_WAIT_MEM);
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
  cmdOut[0]      = Pm4::create(size, Pm4::Custom::R_PUSH_MARKER);
  // memcpy(cmdOut + 1, str, len);

  return Ok;
}

int SYSV_ABI sceGnmInsertPopMarker(uint32_t* cmdOut, uint64_t size) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  cmdOut[0] = Pm4::create(size, Pm4::Custom::R_POP_MARKER);

  return Ok;
}

uint64_t SYSV_ABI sceGnmGetGpuCoreClockFrequency() {
  return 0x800000000;
}

bool SYSV_ABI sceGnmIsUserPaEnabled() {
  return false;
}

bool SYSV_ABI sceRazorIsLoaded() {
  return false;
}

void* SYSV_ABI sceGnmGetTheTessellationFactorRingBufferBaseAddress() {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_TRACE(L"%S", __FUNCTION__);

  return (void*)0xff0000000; // Maps/allocates it afterwards anyway
}

int SYSV_ABI sceGnmValidateCommandBuffers() {
  return Err::VALIDATION_NOT_ENABLED;
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

void SYSV_ABI sceGnmDebugHardwareStatus(int flag) {
  LOG_USE_MODULE(libSceGraphicsDriver);
  LOG_ERR(L"DebugHardwareStatus: Something went wrong");
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
  return SceWorkloadStatus::StatusInvalidPointer;
}

SceWorkloadStatus SYSV_ABI sceGnmEndWorkload(uint64_t workload) {
  return SceWorkloadStatus::StatusOk;
}

SceWorkloadStatus SYSV_ABI sceGnmDestroyWorkloadStream(SceWorkloadStream workloadStream) {
  return SceWorkloadStatus::StatusOk;
}
}
