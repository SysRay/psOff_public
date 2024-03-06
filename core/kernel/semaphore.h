#pragma once
#include "utility/utility.h"

#include <memory>

class ISemaphore {
  CLASS_NO_COPY(ISemaphore);
  CLASS_NO_MOVE(ISemaphore);

  protected:
  ISemaphore() = default;

  public:
  virtual ~ISemaphore() = default;

  virtual int cancel(int setCount, int* numWaitingThreads) = 0;
  virtual int signal(int signalCount)                      = 0;
  virtual int wait(int needcount, uint32_t* pMicros)       = 0;
  virtual int try_wait(int needcount, uint32_t* pMicros)   = 0;

  int poll(int needCount) {
    uint32_t micros = 0;
    return wait(needCount, &micros);
  }

  virtual std::string_view const getName() const = 0;

  virtual size_t getId() const = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL std::unique_ptr<ISemaphore> createSemaphore(const char* name, bool fifo, int initCount, int maxCount);

#undef __APICALL