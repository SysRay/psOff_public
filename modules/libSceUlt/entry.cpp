#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceUlt);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceUlt";

EXPORT SYSV_ABI int32_t sceUltInitialize() {
  return Ok;
}

EXPORT SYSV_ABI size_t sceUltWaitingQueueResourcePoolGetWorkAreaSize(uint32_t numThreads, uint32_t numSyncObjs) {
  return 8;
}

EXPORT SYSV_ABI size_t sceUltQueueDataResourcePoolGetWorkAreaSize(uint32_t numData, size_t dataSize, uint32_t numQueueObjs) {
  return numData * dataSize * numQueueObjs;
}

EXPORT SYSV_ABI int32_t _sceUltWaitingQueueResourcePoolCreate(SceUltWaitingQueueResourcePool* pool, const char* name, uint32_t numThreads,
                                                              uint32_t numSyncObjects, void* workArea, SceUltWaitingQueueResourcePoolOptParam* param) {
  if (pool == nullptr || name == nullptr || workArea == nullptr) return Err::Ult::ERROR_NULL;
  if (numThreads == 0 || numSyncObjects == 0) return Err::Ult::ERROR_INVALID;
  LOG_USE_MODULE(libSceUlt);

  LOG_INFO(L"QueueResourcePoolCreate| name: %S, numThreads: %d, numSyncObjs: %d", name, numThreads, numSyncObjects);
  ::strcpy_s(pool->name, name);
  pool->numThreads     = numThreads;
  pool->numSyncObjects = numSyncObjects;
  pool->workArea       = workArea;
  if (param != nullptr) pool->param = *param;

  return Ok;
}

EXPORT SYSV_ABI int32_t _sceUltQueueDataResourcePoolCreate(SceUltQueueDataResourcePool* pool, const char* name, uint32_t numData, size_t dataSize,
                                                           uint32_t numQueueObjs, SceUltWaitingQueueResourcePool* waitingQueue, void* workArea,
                                                           SceUltQueueDataResourcePoolOptParam* param) {
  if (pool == nullptr || name == nullptr || workArea == nullptr) return Err::Ult::ERROR_NULL;
  if (numData == 0 || dataSize == 0 || numQueueObjs == 0) return Err::Ult::ERROR_INVALID;
  ::strcpy_s(pool->name, name);
  pool->numData         = numData;
  pool->dataSize        = dataSize;
  pool->numQueueObjects = numQueueObjs;
  pool->waitingQueue    = waitingQueue;
  pool->workArea        = workArea;
  pool->queuePtr        = workArea;
  // todo: init mutex
  // todo: init pushEvent
  // todo: init popEvent
  if (param != nullptr) pool->param = *param;
  return Ok;
}

EXPORT SYSV_ABI int32_t _sceUltQueueCreate(SceUltQueue* queue, const char* name, size_t dataSize, SceUltWaitingQueueResourcePool* waitingQueue,
                                           SceUltQueueDataResourcePool* queueData, SceUltQueueOptParam* param) {
  if (queue == nullptr || name == nullptr || queueData == nullptr) return Err::Ult::ERROR_NULL;
  if (dataSize == 0 || dataSize > queueData->dataSize) return Err::Ult::ERROR_INVALID;
  ::strcpy_s(queue->name, name);
  queue->queueData    = queueData;
  queue->waitingQueue = waitingQueue;
  if (param != nullptr) queue->param = *param;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUltQueuePop(SceUltQueue* queue, void* data) {
  LOG_USE_MODULE(libSceUlt);
  LOG_CRIT(L"QueuePop| queue: %p, data: %p", queue, data);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUltQueuePush(SceUltQueue* queue, void* data) {
  LOG_USE_MODULE(libSceUlt);
  LOG_CRIT(L"QueuePush| queue: %p, data: %p", queue, data);
  return Ok;
}

EXPORT SYSV_ABI int32_t _sceUltSemaphoreCreate(SceUltSemaphore* sema, const char* name, uint32_t numResource, SceUltWaitingQueueResourcePool* pool,
                                               SceUltSemaphoreOptParam* param) {
  if (sema == nullptr || name == nullptr) return Err::Ult::ERROR_NULL;
  LOG_USE_MODULE(libSceUlt);

  LOG_INFO(L"SemaphoreCreate| name: %S, numResource: %d", name, numResource);
  ::strcpy_s(sema->name, name);
  sema->handle       = createSemaphore_fifo(name, numResource, 0x7FFFFFFF).release();
  sema->waitingQueue = pool;
  if (param != nullptr) sema->param = *param;

  return Ok;
}

EXPORT SYSV_ABI int32_t _sceUltMutexOptParamInitialize(SceUltMutexOptParam* param) {
  LOG_USE_MODULE(libSceUlt);
  LOG_INFO(L"MutexOptParamInitialize| param: %P", param);
  if (param == nullptr) return Err::Ult::ERROR_NULL;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUltSemaphoreAcquire(SceUltSemaphore* sema, uint32_t numResource) {
  if (sema == nullptr) return Err::Ult::ERROR_NULL;
  if (numResource <= 0 || numResource >= 0x80000000) return Err::Ult::ERROR_RANGE;
  sema->handle->wait(numResource, nullptr);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUltSemaphoreTryAcquire(SceUltSemaphore* sema, uint32_t numResource) {
  if (sema == nullptr) return Err::Ult::ERROR_NULL;
  if (numResource <= 0 || numResource >= 0x80000000) return Err::Ult::ERROR_RANGE;
  if (sema->handle->try_wait(numResource, nullptr) == getErr(ErrCode::_EBUSY)) return Err::Ult::ERROR_BUSY;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUltSemaphoreRelease(SceUltSemaphore* sema, uint32_t numResource) {
  if (sema == nullptr) return Err::Ult::ERROR_NULL;
  if (numResource <= 0 || numResource >= 0x80000000) return Err::Ult::ERROR_RANGE;
  sema->handle->signal(numResource);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUltSemaphoreDestroy(SceUltSemaphore* sema) {
  if (sema == nullptr) return Err::Ult::ERROR_NULL;
  delete sema->handle;
  sema->handle = nullptr;
  return Ok;
}
}
