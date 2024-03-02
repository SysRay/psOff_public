#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libScePlayGo);

namespace {
static uint32_t g_chunks_num = 0;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePlayGo";

EXPORT SYSV_ABI int32_t scePlayGoInitialize(const ScePlayGoInitParams* init) {
  LOG_USE_MODULE(libScePlayGo);

  LOG_INFO(L"bufAddr:0x%08llx bufSize:0x%08x", (uint64_t)init->bufAddr, init->bufSize);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoTerminate(void) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoOpen(ScePlayGoHandle* outHandle, const void* param) {
  LOG_USE_MODULE(libScePlayGo);
  *outHandle = 1;
  // todo
  // if (!Loader::SystemContentGetChunksNum(&g_chunks_num)) {
  LOG_WARN(L"Warning: assume that chunks count is 1");
  g_chunks_num = 1;
  //}

  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoClose(ScePlayGoHandle handle) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetLocus(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries, ScePlayGoLocus* outLoci) {
  LOG_USE_MODULE(libScePlayGo);
  for (uint32_t i = 0; i < numberOfEntries; i++) {
    LOG_DEBUG(L"handle:%d chunk_ids[%u]:%ld", i, handle, chunkIds[i]);

    if (chunkIds[i] <= g_chunks_num) {
      outLoci[i] = 3;
    } else {
      return Err::PLAYGO_ERROR_BAD_CHUNK_ID;
    }
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoSetToDoList(ScePlayGoHandle handle, const ScePlayGoToDo* todoList, uint32_t numberOfEntries) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetToDoList(ScePlayGoHandle handle, ScePlayGoToDo* outTodoList, uint32_t numberOfEntries, uint32_t* outEntries) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoPrefetch(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries, ScePlayGoLocus minimumLocus) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetEta(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries, ScePlayGoEta* outEta) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoSetInstallSpeed(ScePlayGoHandle handle, ScePlayGoInstallSpeed speed) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetInstallSpeed(ScePlayGoHandle handle, ScePlayGoInstallSpeed* speed) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoSetLanguageMask(ScePlayGoHandle handle, ScePlayGoLanguageMask languageMask) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetLanguageMask(ScePlayGoHandle handle, ScePlayGoLanguageMask* outLanguageMask) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetProgress(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries,
                                             ScePlayGoProgress* outProgress) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetChunkId(ScePlayGoHandle handle, ScePlayGoChunkId* outChunkIdList, uint32_t numberOfEntries, uint32_t* outEntries) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceDbgPlayGoRequestNextChunk(ScePlayGoHandle handle, const void* reserved) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceDbgPlayGoSnapshot(ScePlayGoHandle handle, const char* filename) {
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}
}