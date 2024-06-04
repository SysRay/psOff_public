#pragma once

#include "utility/utility.h"

namespace Kernel::EventFlag {

enum class ClearMode { None, All, Bits };

enum class WaitMode { And, Or };

class IKernelEventFlag {
  CLASS_NO_COPY(IKernelEventFlag);
  CLASS_NO_MOVE(IKernelEventFlag);

  protected:
  IKernelEventFlag() = default;

  public:
  virtual ~IKernelEventFlag() = default;

  virtual void set(uint64_t bits)                              = 0;
  virtual void clear(uint64_t bits)                            = 0;
  virtual void cancel(uint64_t bits, int* num_waiting_threads) = 0;

  virtual int wait(uint64_t bits, WaitMode wait_mode, ClearMode clear_mode, uint64_t* result, uint32_t* ptr_micros) = 0;

  virtual int poll(uint64_t bits, WaitMode wait_mode, ClearMode clear_mode, uint64_t* result) = 0;
};

using IKernelEventFlag_t = IKernelEventFlag*;

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL IKernelEventFlag_t createEventFlag(std::string const& name, bool single, bool fifo, uint64_t bits);
__APICALL int                deleteEventFlag(IKernelEventFlag_t ef);

#undef __APICALL
} // namespace Kernel::EventFlag