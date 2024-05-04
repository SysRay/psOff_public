#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceRudp);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceRudp";

EXPORT SYSV_ABI int sceRudpInit(void* memPool, int memPoolSize) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpEnableInternalIOThread(uint32_t stackSize, uint32_t priority) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpEnableInternalIOThread2(uint32_t stackSize, uint32_t priority, uint32_t affinityMask) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetMaxSegmentSize(uint16_t* mss) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpSetEventHandler(SceRudpEventHandler handler, void* arg) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpSetMaxSegmentSize(uint16_t mss) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpCreateContext(SceRudpContextEventHandler handler, void* arg, int* ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetOption(int ctxId, int option, void* optVal, size_t optLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpSetOption(int ctxId, int option, void const* optVal, size_t optLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetContextStatus(int ctxId, SceRudpContextStatus* status, size_t statusSize) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetLocalInfo(int ctxId, int* soc, struct SceNetSockaddr* addr, SceNetSocklen_t* addrLen, uint16_t* vport, uint8_t* muxMode) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetRemoteInfo(int ctxId, struct SceNetSockaddr* addr, SceNetSocklen_t* addrLen, uint16_t* vport) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetStatus(SceRudpStatus* status, size_t statusSize) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpFlush(int ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpNetFlush(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetNumberOfPacketsToRead(int ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetSizeReadable(int ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpGetSizeWritable(int ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpRead(int ctxId, void* data, size_t len, uint8_t flags, SceRudpReadInfo* info) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpWrite(int ctxId, void const* data, size_t len, uint8_t flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpPollCancel(int pollId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpPollControl(int pollId, int op, int ctxId, uint16_t events) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpPollCreate(size_t size) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpPollDestroy(int pollId) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpPollWait(int pollId, SceRudpPollEvent* events, size_t eventLen, uint32_t timeout) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpEnd(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpActivate(int ctxId, struct SceNetSockaddr const* to, SceNetSocklen_t toLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpBind(int ctxId, int soc, uint16_t vport, uint8_t muxMode) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpInitiate(int ctxId, struct SceNetSockaddr const* to, SceNetSocklen_t toLen, uint16_t vport) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpNetReceived(int soc, uint8_t const* data, size_t dataLen, struct SceNetSockaddr const* from, SceNetSocklen_t fromLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpProcessEvents(uint32_t timeout) {
  return Ok;
}

EXPORT SYSV_ABI int sceRudpTerminate(int ctxId) {
  return Ok;
}
}