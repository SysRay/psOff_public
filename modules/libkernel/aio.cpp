#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(aio);

extern "C" {
EXPORT SYSV_ABI int sceKernelAioDeleteRequests(SceKernelAioSubmitId id[], int num, int ret[]) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioDeleteRequest(SceKernelAioSubmitId id, int* ret) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioWaitRequests(SceKernelAioSubmitId id[], int num, int state[], uint32_t mode, SceKernelUseconds* usec) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioWaitRequest(SceKernelAioSubmitId id, int* state, SceKernelUseconds* usec) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioInitialize(SceKernelAioParam* param) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioInitializeImpl(void* p, int size) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioCancelRequests(SceKernelAioSubmitId id[], int num, int state[]) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioCancelRequest(SceKernelAioSubmitId id, int* state) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioPollRequest(SceKernelAioSubmitId id, int* state) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioPollRequests(SceKernelAioSubmitId id[], int num, int state[]) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioSubmitReadCommands(SceKernelAioRWRequest req[], int size, int prio, SceKernelAioSubmitId* id) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioSubmitReadCommandsMultiple(SceKernelAioRWRequest req[], int size, int prio, SceKernelAioSubmitId id[]) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioSubmitWriteCommands(SceKernelAioRWRequest req[], int size, int prio, SceKernelAioSubmitId* id) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAioSubmitWriteCommandsMultiple(SceKernelAioRWRequest req[], int size, int prio, SceKernelAioSubmitId id[]) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceKernelAioInitializeParam(SceKernelAioParam* param) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI int sceKernelAioSetParam(SceKernelAioSchedulingParam* param, int schedulingWindowSize, int handleelayedCountLimit, uint32_t enableSplit,
                                         uint32_t splitSize, uint32_t splitChunkSize) {
  LOG_USE_MODULE(aio);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}