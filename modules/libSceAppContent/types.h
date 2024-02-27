#pragma once
#include "../libSceNpManager/types.h"
#include "codes.h"


typedef uint32_t SceAppContentMediaType;
typedef uint32_t SceAppContentBootAttribute;
typedef uint32_t SceAppContentAppParamId;
typedef uint32_t SceAppContentAddcontDownloadStatus;
typedef uint32_t SceAppContentTemporaryDataOption;

enum class AppParamId : uint32_t { SKU_FLAG = 0, USER_DEFINED_PARAM_1 = 1, USER_DEFINED_PARAM_2 = 2, USER_DEFINED_PARAM_3 = 3, USER_DEFINED_PARAM_4 = 4 };

enum class SkuFlag : uint32_t { TRIAL = 1, FULL = 3 };

enum class AddContDownloadStatus : uint32_t { NO_EXTRA_DATA = 0, NO_IN_QUEUE = 1, DOWNLOADING = 2, DOWNLOAD_SUSPENDED = 3, INSTALLED = 4 };

enum class TemporaryDataOption : uint32_t { NONE = 0, FORMAT = (1 << 0) };

struct SceAppContentInitParam {
  char reserved[32];
};

struct SceAppContentBootParam {
  char                       reserved1[4];
  SceAppContentBootAttribute attr;
  char                       reserved2[32];
};

struct SceAppContentAddcontInfo {
  SceNpUnifiedEntitlementLabel       entitlementLabel;
  SceAppContentAddcontDownloadStatus status;
};

struct SceAppContentMountPoint {
  char data[MOUNTPOINT_DATA_MAXSIZE];
};

struct SceAppContentEntitlementKey {
  char data[ENTITLEMENT_KEY_SIZE];
};

struct SceAppContentAddcontDownloadProgress {
  uint64_t dataSize;
  uint64_t downloadedSize;
};