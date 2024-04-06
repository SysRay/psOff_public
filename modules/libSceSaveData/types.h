#pragma once
#include "codes.h"

enum class SceSaveDataMountMode : uint32_t {
  READ_ONLY    = 0x00000001,
  READ_WRITE   = 0x00000002,
  CREATE       = 0x00000004,
  DESTRUCT_OFF = 0x00000008,
  COPY_ICON    = 0x00000010,
  CREATE2      = 0x00000020
};

enum class SceSaveDataMountStatus : uint32_t { CREATED = 0x00000001 };

enum class SceSaveDataParamType : uint32_t { ALL = 0, TITLE = 1, SUB_TITLE = 2, DETAIL = 3, USER_PARAM = 4, MTIME = 5 };

enum class SceSaveDataSortKey : uint32_t { DIRNAME = 0, USER_PARAM = 1, BLOCKS = 2, MTIME = 3, FREE_BLOCKS = 5 };

enum class SceSaveDataSortOrder : uint32_t { ASCENT = 0, DESCENT = 1 };

enum class SceSaveDataMemorySyncOption : uint32_t { NONE = 0x00000000, BLOCKING = 0x00000001 << 0 };

enum class SceSaveDataEventType : uint32_t { INVALID = 0, UMOUNT_BACKUP_END = 1, BACKUP_END = 2, SAVE_DATA_MEMORY_SYNC_END = 3 };

union SceSaveDataMemoryOption {
  struct {
    uint32_t SET_PARAM     : 1;
    uint32_t DOUBLE_BUFFER : 1;
  } bits;

  uint32_t bitfield;
};

/*
 * Structures
 */

struct SceSaveDataTitleId {
  char data[SCE_SAVE_DATA_TITLE_ID_DATA_SIZE];
  char padding[6];
};

struct SceSaveDataDirName {
  char data[SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE];
};

struct SceSaveDataMountPoint {
  char data[SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE];
};

struct SceSaveDataFingerprint {
  char data[SCE_SAVE_DATA_FINGERPRINT_DATA_SIZE];
  char padding[15];
};

struct SceSaveDataInitParams3;

struct SceSaveDataMount {
  int         userId;
  int         pad;
  const char* titleId;
  const char* dirName;
  const char* fingerprint;
  uint64_t    blocks;

  SceSaveDataMountMode mountMode;
  uint8_t              reserved[32];
};

struct SceSaveDataMount2 {
  int32_t userId;
  int : 32;
  const SceSaveDataDirName* dirName;
  SceSaveDataBlocks         blocks;

  SceSaveDataMountMode mountMode;
  uint8_t              reserved[32];
  int : 32;
};

struct SceSaveDataTransferringMount {
  int32_t                       userId;
  const SceSaveDataTitleId*     titleId;
  const SceSaveDataDirName*     dirName;
  const SceSaveDataFingerprint* fingerprint;
  uint8_t                       reserved[32];
};

struct SceSaveDataMountResult {
  SceSaveDataMountPoint mountPoint;

  SceSaveDataBlocks requiredBlocks;
  uint32_t          unused;
  uint32_t          mountStatus;
  uint8_t           reserved[28];
  int : 32;
};

struct SceSaveDataMountInfo {
  SceSaveDataBlocks blocks;
  SceSaveDataBlocks freeBlocks;
  uint8_t           reserved[32];
};

struct SceSaveDataIcon {
  void*   buf;
  size_t  bufSize;
  size_t  dataSize;
  uint8_t reserved[32];
};

struct SceSaveDataParam {
  char title[SCE_SAVE_DATA_TITLE_MAXSIZE];
  char subTitle[SCE_SAVE_DATA_SUBTITLE_MAXSIZE];

  char detail[SCE_SAVE_DATA_DETAIL_MAXSIZE];

  uint32_t userParam;
  int : 32;
  uint64_t mtime;
  uint8_t  reserved[32];
};

struct SceSaveDataDelete {
  int32_t userId;
  int : 32;
  const SceSaveDataTitleId* titleId;
  const SceSaveDataDirName* dirName;
  uint32_t                  unused;
  uint8_t                   reserved[32];
  int : 32;
};

struct SceSaveDataSearchInfo {
  SceSaveDataBlocks blocks;
  SceSaveDataBlocks freeBlocks;
  uint8_t           reserved[32];
};

struct SceSaveDataDirNameSearchCond {
  int32_t userId;
  int : 32;
  const SceSaveDataTitleId* titleId;
  const SceSaveDataDirName* dirName;
  SceSaveDataSortKey        key;
  SceSaveDataSortOrder      order;
  uint8_t                   reserved[32];
};

struct SceSaveDataDirNameSearchResult {
  uint32_t hitNum;
  int : 32;
  SceSaveDataDirName*    dirNames;
  uint32_t               dirNamesNum;
  uint32_t               setNum;
  SceSaveDataParam*      params;
  SceSaveDataSearchInfo* infos;
  uint8_t                reserved[12];
  int : 32;
};

struct SceSaveDataMemorySync {
  int32_t                     userId;
  uint32_t                    slotId; // Values: 0...3
  SceSaveDataMemorySyncOption option;
  uint8_t                     reserved[28];
};

struct SceSaveDataRestoreBackupData {
  int32_t userId;
  int : 32;
  const SceSaveDataTitleId*     titleId;
  const SceSaveDataDirName*     dirName;
  const SceSaveDataFingerprint* fingerprint;
  uint32_t                      unused;
  uint8_t                       reserved[32];
  int : 32;
};

struct SceSaveDataCheckBackupData {
  int32_t userId;
  int : 32;
  const SceSaveDataTitleId* titleId;
  const SceSaveDataDirName* dirName;
  SceSaveDataParam*         param;
  SceSaveDataIcon*          icon;
  uint8_t                   reserved[32];
};

struct SceSaveDataBackup {
  int32_t userId;
  int : 32;
  const SceSaveDataTitleId*     titleId;
  const SceSaveDataDirName*     dirName;
  const SceSaveDataFingerprint* fingerprint;
  uint8_t                       reserved[32];
};

struct SceSaveDataMemoryData {
  void*   buf;
  size_t  bufSize;
  int64_t offset;
  uint8_t reserved[40];
};

struct SceSaveDataMemorySetup2 {
  SceSaveDataMemoryOption option;
  int32_t                 userId;
  size_t                  memorySize;
  size_t                  iconMemorySize;
  const SceSaveDataParam* initParam;
  const SceSaveDataIcon*  initIcon;
  uint32_t                slotId; // Values: 0...3
  uint8_t                 reserved[20];
};

struct SceSaveDataMemorySetupResult {
  size_t  existedMemorySize;
  uint8_t reserved[16];
};

struct SceSaveDataMemoryGet2 {
  int32_t                userId;
  uint8_t                padding[4];
  SceSaveDataMemoryData* data;
  SceSaveDataParam*      param;
  SceSaveDataIcon*       icon;
  uint32_t               slotId; // Values: 0...3
  uint8_t                reserved[28];
};

struct SceSaveDataMemorySet2 {
  int32_t                      userId;
  uint8_t                      padding[4];
  const SceSaveDataMemoryData* data;
  const SceSaveDataParam*      param;
  const SceSaveDataIcon*       icon;
  uint32_t                     dataNum;
  uint32_t                     slotId; // Values: 0...3
  uint8_t                      reserved[24];
};

struct SceSaveDataEventParam;

struct SceSaveDataEvent {
  SceSaveDataEventType type;
  int32_t              errorCode;
  int32_t              userId;
  uint8_t              padding[4];
  SceSaveDataTitleId   titleId;
  SceSaveDataDirName   dirName;
  uint8_t              reserved[40];
};
