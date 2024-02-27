#pragma once
#include "utility/utility.h"

struct SceAvPlayerFrameInfo;
struct SceAvPlayerMemAllocator;

class IAvplayer {
  CLASS_NO_COPY(IAvplayer);

  protected:
  IAvplayer() = default;

  public:
  virtual bool setFile(const char* filename) = 0;
  virtual void setLoop(bool isLoop)          = 0;

  virtual bool getVideoData(void* videoInfo, bool isEx)      = 0;
  virtual bool getAudioData(SceAvPlayerFrameInfo* videoInfo) = 0;

  virtual bool isPlaying() = 0;
  virtual void stop()      = 0;

  virtual ~IAvplayer() = default;
};

std::unique_ptr<IAvplayer> createAvPlayer(SceAvPlayerMemAllocator const& initData);