#pragma once
#include "codes.h"

struct SceUltWaitingQueueResourcePoolOptParam {
  char _unknown[128];
};

struct SceUltSemaphoreOptParam {
  char _unknown[128];
};

struct SceUltQueueDataResourcePoolOptParam {
  char _unknown[128];
};

struct SceUltQueueOptParam {
  char _unknown[128];
};

struct SceUltWaitingQueueResourcePool {
  SceUltWaitingQueueResourcePoolOptParam param;
  char                                   name[SCE_ULT_MAX_NAME_LENGTH];
  uint32_t                               numThreads;
  uint32_t                               numSyncObjects;
  void*                                  workArea;
  char                                   _unknown[76];
};

class ISemaphore;

struct SceUltSemaphore {
  SceUltSemaphoreOptParam         param;
  char                            name[SCE_ULT_MAX_NAME_LENGTH];
  SceUltWaitingQueueResourcePool* waitingQueue;
  ISemaphore*                     handle;
  char                            _unknown[76];
};

struct SceUltQueueDataResourcePool {
  SceUltQueueDataResourcePoolOptParam param;
  uint32_t                            numData;
  uint32_t                            numQueueObjects;
  size_t                              dataSize;
  SceUltWaitingQueueResourcePool*     waitingQueue;
  void*                               workArea;
  char                                name[SCE_ULT_MAX_NAME_LENGTH];
  void*                               mutex; // ???
  void*                               queuePtr;
  void*                               pushEvent; // cond var
  void*                               popEvent;  // cond var
  char                                _unknown[288];
};

struct SceUltQueue {
  SceUltQueueOptParam             param;
  char                            name[SCE_ULT_MAX_NAME_LENGTH];
  SceUltQueueDataResourcePool*    queueData;
  SceUltWaitingQueueResourcePool* waitingQueue;
  char                            _unknown[336];
};

struct SceUltUlthreadOptParam {
  uint32_t attr;
  char     _unknown[127 - 4];
};

struct SceUltMutexOptParam {
  char _unknown[128];
};