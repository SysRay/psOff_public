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

  virtual int cancel(int setCount, int* numCanceled)     = 0;
  virtual int signal(int signalCount)                    = 0;
  virtual int wait(int needcount, uint32_t* pMicros)     = 0;
  virtual int try_wait(int needcount, uint32_t* pMicros) = 0;
  virtual int poll(int needCount)                        = 0;

  virtual std::string_view const getName() const = 0;

  virtual size_t getId() const = 0;

  virtual size_t getSignalCounter() const = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL std::unique_ptr<ISemaphore> createSemaphore_fifo(const char* name, int initCount, int maxCount);
__APICALL std::unique_ptr<ISemaphore> createSemaphore_prio(const char* name, int initCount, int maxCount);

#undef __APICALL