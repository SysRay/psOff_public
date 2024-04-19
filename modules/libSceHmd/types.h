#pragma once
#include "codes.h"
#include "sceTypes.h"

enum class SceHmdReprojectionReprojectionType {};

struct SceHmdInitializeParam {
  void*   reserved0;
  uint8_t reserved[8];
};

struct SceHmdReprojectionResourceInfo {
  void*    pOnionBuff;
  void*    pGarlicBuff;
  int      threadPriority;
  uint64_t cpuAffinityMask;
  uint32_t pipeId;
  uint32_t queueId;
  uint32_t reserved[3];
};

struct SceHmdDeviceInformation {
  uint32_t status;
  uint8_t  reserve0[4], userId;

  struct DeviceInfo {
    struct PanelResolution {
      uint32_t width;
      uint32_t height;
    } panelResolution;

    struct FlipToDisplayLatency {
      uint16_t refreshRate90Hz;
      uint16_t refreshRate120Hz;
    } flipToDisplayLatency;

  } deviceInfo;

  uint8_t hmuMount;
  uint8_t reserve1[7];
};

struct SceHmdOpenParam {
  uint8_t reserve[8];
};

struct SceHmdFieldOfView {
  float tanOut;
  float tanIn;
  float tanTop;
  float tanBottom;
};

struct SceHmdEyeOffset {
  SceFVector3 offset;
  uint8_t     reserve[20];
};