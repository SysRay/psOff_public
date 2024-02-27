#pragma once
#include "codes.h"

struct SceSystemServiceEvent {
  SceSystemServiceEventType eventType;
  union {
    char param[8192];
    struct {
      char source[1024];
      union {
        char arg[4096];
        char url[4096];
      };
    } urlOpen;
    struct {
      uint32_t size;
      uint8_t  arg[8188];
    } launchApp;
    struct {
      uint32_t size;
      uint8_t  arg[2020];
    } appLaunchLink;
    struct {
      int32_t userId;
      char    eventId[37];
      char    bootArgument[7169];
    } joinEvent;
    struct {
      int32_t  userId;
      uint32_t npServiceLabel;
      uint8_t  reserved[8184];
    } serviceEntitlementUpdate;
    struct {
      int32_t  userId;
      uint32_t npServiceLabel;
      uint8_t  reserved[8184];
    } unifiedEntitlementUpdate;
    uint8_t reserved[8192];
  } data;
};

struct SceSystemServiceStatus {
  int32_t eventNum                 = 0;
  bool    isSystemUiOverlaid       = false;
  bool    isInBackgroundExecution  = false;
  bool    isCpuMode7CpuNormal      = true;
  bool    isGameLiveStreamingOnAir = false;
  bool    isOutOfVrPlayArea        = false;
  uint8_t reserved[125];
};

struct SceSystemServiceDisplaySafeAreaInfo {
  float   ratio;
  uint8_t reserved[128];
};

struct SceSystemServiceLaunchWebBrowserParam {
  uint32_t size;
  uint8_t  reserved[128];
};

struct SceSystemServiceHdrToneMapLuminance {
  float maxFullFrameToneMapLuminance;
  float maxToneMapLuminance;
  float minToneMapLuminance;
};