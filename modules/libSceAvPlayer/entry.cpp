#include "avplayer.h"
#include "common.h"
#include "logging.h"
#include "types.h"
LOG_DEFINE_MODULE(libSceAvPlayer);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAvPlayer";

EXPORT SYSV_ABI SceAvPlayerHandle sceAvPlayerInit(SceAvPlayerInitData* pInit) {
  auto inst = createAvPlayer(pInit->memoryReplacement);
  return inst.release();
}

EXPORT SYSV_ABI int32_t sceAvPlayerPostInit(IAvplayer* avPlayer, SceAvPlayerPostInitData* pPostInit) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerClose(IAvplayer* avPlayer) {
  delete avPlayer;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerAddSource(IAvplayer* avPlayer, const char* filename) {
  if (avPlayer->setFile(filename)) return Ok;
  return Err::AvPlayer::INVALID_PARAMS;
}

EXPORT SYSV_ABI int32_t sceAvPlayerStreamCount(IAvplayer* avPlayer) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerGetStreamInfo(IAvplayer* avPlayer, uint32_t argStreamID, SceAvPlayerStreamInfo* argInfo) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerEnableStream(IAvplayer* avPlayer, uint32_t argStreamID) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerDisableStream(IAvplayer* avPlayer, uint32_t argStreamID) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerChangeStream(IAvplayer* avPlayer, uint32_t argOldStreamID, uint32_t argNewStreamID) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerStart(IAvplayer* avPlayer) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerStop(IAvplayer* avPlayer) {
  avPlayer->stop();
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerPause(IAvplayer* avPlayer) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerResume(IAvplayer* avPlayer) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerSetLooping(IAvplayer* avPlayer, bool loopFlag) {
  avPlayer->setLoop(loopFlag);
  return Ok;
}

EXPORT SYSV_ABI bool sceAvPlayerIsActive(IAvplayer* avPlayer) {
  if (avPlayer != nullptr) return avPlayer->isPlaying();
  return false;
}

EXPORT SYSV_ABI bool sceAvPlayerGetAudioData(IAvplayer* avPlayer, SceAvPlayerFrameInfo* audioInfo) {
  return avPlayer->getAudioData(audioInfo);
}

EXPORT SYSV_ABI bool sceAvPlayerGetVideoData(IAvplayer* avPlayer, SceAvPlayerFrameInfo* videoInfo) {
  return avPlayer->getVideoData(videoInfo, false);
}

EXPORT SYSV_ABI uint64_t sceAvPlayerCurrentTime(IAvplayer* avPlayer) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerJumpToTime(IAvplayer* avPlayer, uint64_t argJumpTimeMsec) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerSetTrickSpeed(IAvplayer* avPlayer, int32_t argTrickSpeed) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerSetAvSyncMode(IAvplayer* avPlayer, SceAvPlayerAvSyncMode argSyncMode) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerSetLogCallback(SceAvPlayerLogCallback logCb, void* userData) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerPrintf(const char* format, ...) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAvPlayerVprintf(const char* str, va_list args) {
  LOG_USE_MODULE(libSceAvPlayer);
  std::string format(str);
  LOG_DEBUG(std::wstring(format.begin(), format.end()).c_str(), args);
  return Ok;
}
}
