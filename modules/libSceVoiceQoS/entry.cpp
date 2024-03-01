#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceVoiceQoS);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceVoiceQoS";

EXPORT SYSV_ABI int32_t sceVoiceQoSInit(void* pMemBlock, uint32_t memSize, int32_t appType) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSEnd(void) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSCreateLocalEndpoint(SceVoiceQoSLocalId* pLocalId, int32_t userId, int32_t deviceInId, int32_t deviceOutId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSDeleteLocalEndpoint(SceVoiceQoSLocalId localId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSCreateRemoteEndpoint(SceVoiceQoSRemoteId* pRemoteId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSDeleteRemoteEndpoint(SceVoiceQoSRemoteId remoteId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSConnect(SceVoiceQoSConnectionId* pConnectionId, SceVoiceQoSLocalId localId, SceVoiceQoSRemoteId remoteId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSDisconnect(SceVoiceQoSConnectionId connectionId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSGetLocalEndpoint(SceVoiceQoSConnectionId connectionId, SceVoiceQoSLocalId* pLocalId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSGetRemoteEndpoint(SceVoiceQoSConnectionId connectionId, SceVoiceQoSRemoteId* pRemoteId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSSetLocalEndpointAttribute(SceVoiceQoSLocalId localId, SceVoiceQoSAttributeId attributeId, const void* pAttributeValue,
                                                             int attributeSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSGetLocalEndpointAttribute(SceVoiceQoSLocalId localId, SceVoiceQoSAttributeId attributeId, void* pAttributeValue,
                                                             int attributeSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSSetRemoteEndpointAttribute(SceVoiceQoSRemoteId remoteId, SceVoiceQoSAttributeId attributeId, const void* pAttributeValue,
                                                              int attributeSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSGetRemoteEndpointAttribute(SceVoiceQoSRemoteId remoteId, SceVoiceQoSAttributeId attributeId, const void* pAttributeValue,
                                                              int attributeSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSSetConnectionAttribute(SceVoiceQoSConnectionId connectionId, SceVoiceQoSAttributeId attributeId, const void* pAttributeValue,
                                                          int attributeSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSGetConnectionAttribute(SceVoiceQoSConnectionId connectionId, SceVoiceQoSAttributeId attributeId, void* pAttributeValue,
                                                          int attributeSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSGetStatus(SceVoiceQoSConnectionId connectionId, SceVoiceQoSStatusId statusId, void* pStatusValue, int statusSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSWritePacket(SceVoiceQoSConnectionId connectionId, const void* pData, unsigned int* pSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceVoiceQoSReadPacket(SceVoiceQoSConnectionId connectionId, void* pData, unsigned int* pSize) {
  return Ok;
}
}