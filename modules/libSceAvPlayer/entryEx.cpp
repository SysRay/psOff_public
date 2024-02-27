#include "avplayer.h"
#include "common.h"
#include "typesEx.h"

#include <logging.h>

namespace {} // namespace

extern "C" {
EXPORT SYSV_ABI int32_t sceAvPlayerInitEx(const SceAvPlayerInitDataEx* initDataEx, IAvplayer** avPlayer) {
  auto inst = createAvPlayer(initDataEx->memoryReplacement);
  *avPlayer = inst.release();

  return Ok;
}
EXPORT SYSV_ABI bool sceAvPlayerGetVideoDataEx(IAvplayer* avPlayer, SceAvPlayerFrameInfoEx* videoInfo) {
  return avPlayer->getVideoData(videoInfo, true);
}
EXPORT SYSV_ABI int32_t sceAvPlayerAddSourceEx(IAvplayer* avPlayer, SceAvPlayerUriType uriType, SceAvPlayerSourceDetails* sourceDetails) {
  if (avPlayer->setFile(sourceDetails->uri.name)) return Ok;
  return Err::INVALID_PARAMS;
}
}