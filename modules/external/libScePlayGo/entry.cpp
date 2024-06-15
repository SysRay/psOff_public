#include "chunkreader.hpp"
#include "common.h"
#include "core/fileManager/fileManager.h"
#include "logging.h"
#include "types.h"

#include <chrono>
#include <mutex>

LOG_DEFINE_MODULE(libScePlayGo);

namespace {
static GoReader*             g_goread = nullptr;
static std::mutex            g_ispeedmtx;
static ScePlayGoInstallSpeed g_currspeed  = 0;
static int64_t               g_currspeedt = 0ll;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePlayGo";

EXPORT SYSV_ABI int32_t scePlayGoInitialize(const ScePlayGoInitParams* init) {
  if (g_goread != nullptr) return Err::PlayGo::ALREADY_INITIALIZED;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"bufAddr:0x%08llx bufSize:0x%08x", (uint64_t)init->bufAddr, init->bufSize);
  auto path = accessFileManager().getMappedPath("/app0/sce_sys/playgo-chunk.dat");
  g_goread  = new GoReader(*path);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoTerminate(void) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"Terminating..");
  delete g_goread;
  g_goread = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoOpen(ScePlayGoHandle* outHandle, const void* param) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"outHandle:0x%08llx param:0x%08x", (uint64_t)outHandle, (uint64_t)param);
  *outHandle = 1;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoClose(ScePlayGoHandle handle) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetLocus(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries, ScePlayGoLocus* outLoci) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!chunkIds || !outLoci) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;
  LOG_USE_MODULE(libScePlayGo);
  for (uint32_t i = 0; i < numberOfEntries; i++) {
    LOG_DEBUG(L"handle:%d chunk_ids[%u]:%ld", i, handle, chunkIds[i]);

    if (chunkIds[i] < g_goread->getChunksCount()) {
      outLoci[i] = (uint8_t)ScePlayGoLocusValue::LocalFast;
    } else {
      outLoci[i] = (uint8_t)ScePlayGoLocusValue::NotDownloaded;
      return Err::PlayGo::BAD_CHUNK_ID;
    }
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoSetToDoList(ScePlayGoHandle handle, const ScePlayGoToDo* todoList, uint32_t numberOfEntries) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!todoList) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetToDoList(ScePlayGoHandle handle, ScePlayGoToDo* outTodoList, uint32_t numberOfEntries, uint32_t* outEntries) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!outTodoList) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  *outEntries = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoPrefetch(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries, ScePlayGoLocus minimumLocus) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!chunkIds) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;

  switch ((ScePlayGoLocusValue)minimumLocus) {
    case ScePlayGoLocusValue::NotDownloaded:
    case ScePlayGoLocusValue::LocalSlow:
    case ScePlayGoLocusValue::LocalFast: break;

    default: return Err::PlayGo::BAD_LOCUS;
  }
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetEta(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries, ScePlayGoEta* outEta) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!chunkIds || !outEta) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"ETA set to 0");

  *outEta = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoSetInstallSpeed(ScePlayGoHandle handle, ScePlayGoInstallSpeed speed) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"handle: %d, speed: %d", handle, speed);

  std::unique_lock lock(g_ispeedmtx);

  g_currspeed  = speed;
  g_currspeedt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::now().time_since_epoch()).count();

  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetInstallSpeed(ScePlayGoHandle handle, ScePlayGoInstallSpeed* speed) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"handle: %d, speed: %08llx", handle, (uint64_t)speed);

  std::unique_lock lock(g_ispeedmtx);

  if (g_currspeed == 0) {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::now().time_since_epoch()).count();
    if ((now - g_currspeedt) > 30000 /* 30 seconds */) {
      g_currspeed = (ScePlayGoInstallSpeed)ScePlayGoInstallSpeedValue::Trickle;
    }
  }

  *speed = g_currspeed;

  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoSetLanguageMask(ScePlayGoHandle handle, ScePlayGoLanguageMask languageMask) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetLanguageMask(ScePlayGoHandle handle, ScePlayGoLanguageMask* outLanguageMask) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetProgress(ScePlayGoHandle handle, const ScePlayGoChunkId* chunkIds, uint32_t numberOfEntries,
                                             ScePlayGoProgress* outProgress) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!chunkIds || !outProgress) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;

  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"handle: %d, chunkIds: %08llx, numEnts: %lu, outProgress: %08llx", handle, (uint64_t)chunkIds, numberOfEntries, (uint64_t)outProgress);

  outProgress->progressSize = 0;
  outProgress->totalSize    = 0;

  uint64_t total = 0;
  for (int i = 0; i < numberOfEntries; ++i) {
    if (auto chunk_size = g_goread->getChunkSize(chunkIds[i])) {
      total += chunk_size;
      continue;
    }

    return Err::PlayGo::BAD_CHUNK_ID;
  }

  outProgress->progressSize = total;
  outProgress->totalSize    = total;

  return Ok;
}

EXPORT SYSV_ABI int32_t scePlayGoGetChunkId(ScePlayGoHandle handle, ScePlayGoChunkId* outChunkIdList, uint32_t numberOfEntries, uint32_t* outEntries) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  if (!outChunkIdList || !outEntries) return Err::PlayGo::BAD_POINTER;
  if (!numberOfEntries) return Err::PlayGo::BAD_SIZE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_INFO(L"handle: %d, outChunksList: %08llx, numEnts: %lu, outEnts: %08llx", handle, (uint64_t)outChunkIdList, numberOfEntries, (uint64_t)outEntries);

  *outEntries = std::min(g_goread->getChunksCount(), numberOfEntries);

  for (int i = 0; i < *outEntries; ++i) {
    outChunkIdList[i] = i;
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceDbgPlayGoRequestNextChunk(ScePlayGoHandle handle, const void* reserved) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceDbgPlayGoSnapshot(ScePlayGoHandle handle, const char* filename) {
  if (!g_goread) return Err::PlayGo::NOT_INITIALIZED;
  if (handle != 1) return Err::PlayGo::BAD_HANDLE;
  LOG_USE_MODULE(libScePlayGo);
  LOG_ERR(L"TODO: %S", __FUNCTION__);
  return Ok;
}
}
