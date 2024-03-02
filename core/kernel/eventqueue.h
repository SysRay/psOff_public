#pragma once
#include "eventqueue_types.h"
#include "modules_include/common.h"
#include "utility/utility.h"

namespace Kernel::EventQueue {
class IKernelEqueue {
  CLASS_NO_COPY(IKernelEqueue);
  CLASS_NO_MOVE(IKernelEqueue);

  protected:
  IKernelEqueue() = default;

  public:
  virtual ~IKernelEqueue() = default;

  virtual int wait(KernelEvent_t ev, int num, int* out, const SceKernelUseconds* timo) = 0;

  virtual int addUserEvent(int ident)                        = 0;
  virtual int addUserEventEdge(int ident)                    = 0;
  virtual int triggerUserEvent(uintptr_t ident, void* udata) = 0;

  virtual int addEvent(const KernelEqueueEvent& event)                          = 0;
  virtual int triggerEvent(uintptr_t ident, int16_t filter, void* trigger_data) = 0;
  virtual int deleteEvent(uintptr_t ident, int16_t filter)                      = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL int createEqueue(IKernelEqueue_t* eq, const char* name);
__APICALL int deleteEqueue(IKernelEqueue_t eq);

#undef __APICALL

} // namespace Kernel::EventQueue