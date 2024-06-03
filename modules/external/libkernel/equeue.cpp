#include "common.h"
#include "core/kernel/eventqueue.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(equeue);

extern "C" {
EXPORT SYSV_ABI int sceKernelGetEventFilter(const Kernel::EventQueue::KernelEvent_t ev) {
  return ev->filter;
}

EXPORT SYSV_ABI uintptr_t sceKernelGetEventId(const Kernel::EventQueue::KernelEvent_t ev) {
  return ev->ident;
}

EXPORT SYSV_ABI intptr_t sceKernelGetEventData(const Kernel::EventQueue::KernelEvent_t ev) {
  return ev->data;
}

EXPORT SYSV_ABI unsigned int sceKernelGetEventFflags(const Kernel::EventQueue::KernelEvent_t ev) {
  return ev->fflags;
}

EXPORT SYSV_ABI int sceKernelGetEventError(const Kernel::EventQueue::KernelEvent_t ev) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void* sceKernelGetEventUserData(const Kernel::EventQueue::KernelEvent_t ev) {
  return ev->udata;
}

EXPORT SYSV_ABI int sceKernelCreateEqueue(Kernel::EventQueue::IKernelEqueue_t* eq, const char* name) {
  return Kernel::EventQueue::createEqueue(eq, name);
}

EXPORT SYSV_ABI int sceKernelDeleteEqueue(Kernel::EventQueue::IKernelEqueue_t eq) {
  return Kernel::EventQueue::deleteEqueue(eq);
}

EXPORT SYSV_ABI int sceKernelWaitEqueue(Kernel::EventQueue::IKernelEqueue_t eq, Kernel::EventQueue::KernelEvent_t ev, int num, int* out,
                                        SceKernelUseconds* time) {
  return eq->wait(ev, num, out, time);
}

EXPORT SYSV_ABI int sceKernelAddTimerEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id, SceKernelUseconds usec, void* udata) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteTimerEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAddReadEvent(Kernel::EventQueue::IKernelEqueue_t eq, int fd, size_t size, void* udata) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteReadEvent(Kernel::EventQueue::IKernelEqueue_t eq, int fd) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAddWriteEvent(Kernel::EventQueue::IKernelEqueue_t eq, int fd, size_t size, void* udata) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteWriteEvent(Kernel::EventQueue::IKernelEqueue_t eq, int fd) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAddFileEvent(Kernel::EventQueue::IKernelEqueue_t eq, int fd, int watch, void* udata) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteFileEvent(Kernel::EventQueue::IKernelEqueue_t eq, int fd) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelAddUserEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id) {
  return eq->addUserEvent(id);
}

EXPORT SYSV_ABI int sceKernelAddUserEventEdge(Kernel::EventQueue::IKernelEqueue_t eq, int id) {
  return eq->addUserEventEdge(id);
}

EXPORT SYSV_ABI int sceKernelDeleteUserEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id) {
  return eq->deleteEvent(id, Kernel::EventQueue::KERNEL_EVFILT_USER);
}

EXPORT SYSV_ABI int sceKernelTriggerUserEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id, void* udata) {
  return eq->triggerUserEvent(id, udata);
}

EXPORT SYSV_ABI int sceKernelAddHRTimerEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id, SceKernelTimespec* ts, void* udata) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteHRTimerEvent(Kernel::EventQueue::IKernelEqueue_t eq, int id) {
  LOG_USE_MODULE(equeue);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}