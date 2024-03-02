#include "../libSceNpManager/types.h"
#include "common.h"
#include "core/fileManager/fileManager.h"
#include "core/systemContent/systemContent.h"
#include "logging.h"
#include "types.h"

#include <filesystem>
#include <optional>

LOG_DEFINE_MODULE(libSceAppContent);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAppContentUtil";

EXPORT SYSV_ABI int32_t sceAppContentInitialize(SceAppContentInitParam* initParam, SceAppContentBootParam* bootParam) {
  bootParam->attr = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentAppParamGetInt(SceAppContentAppParamId paramId, int32_t* value) {
  LOG_USE_MODULE(libSceAppContent);

  *value = 0;
  std::optional<uint32_t> retValue;

  switch (paramId) {
    case 0: retValue = 3; break;
    case 1: retValue = accessSystemContent().getInt("USER_DEFINED_PARAM_1"); break;
    case 2: retValue = accessSystemContent().getInt("USER_DEFINED_PARAM_2"); break;
    case 3: retValue = accessSystemContent().getInt("USER_DEFINED_PARAM_3"); break;
    case 4: retValue = accessSystemContent().getInt("USER_DEFINED_PARAM_4"); break;
    default: LOG_CRIT(L"unknown param_id:%u", paramId);
  }

  if (retValue) *value = (int32_t)retValue.value();
  LOG_TRACE(L"paramId:%u value:%d [%S]", paramId, *value, retValue.has_value() ? "found" : "not found");

  return retValue.has_value() ? Ok : Err::PARAMETER;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentGetAddcontInfoList(SceNpServiceLabel serviceLabel, SceAppContentAddcontInfo* list, uint32_t listNum, uint32_t* hitNum) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *hitNum = 0;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentGetAddcontInfo(SceNpServiceLabel serviceLabel, const SceNpUnifiedEntitlementLabel* entitlementLabel,
                                                    SceAppContentAddcontInfo* info) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentAddcontMount(SceNpServiceLabel serviceLabel, const SceNpUnifiedEntitlementLabel* entitlementLabel,
                                                  SceAppContentMountPoint* mountPoint) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentAddcontUnmount(const SceAppContentMountPoint* mountPoint) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentAddcontEnqueueDownload(SceNpServiceLabel serviceLabel, const SceNpUnifiedEntitlementLabel* entitlementLabel) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentGetEntitlementKey(SceNpServiceLabel serviceLabel, const SceNpUnifiedEntitlementLabel* entitlementLabel,
                                                       SceAppContentEntitlementKey* key) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentTemporaryDataMount2(SceAppContentTemporaryDataOption option, SceAppContentMountPoint* mountPoint) {
  auto tempFolder = accessFileManager().getMountPoint(MountType::Temp, MOUNT_POINT_TEMP.data());
  if ((option & 1) > 0) {
    std::filesystem::remove_all(tempFolder);
  }

  auto const count        = MOUNT_POINT_TEMP.copy(mountPoint->data, MOUNTPOINT_DATA_MAXSIZE - 1);
  mountPoint->data[count] = '\0';

  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentTemporaryDataUnmount(const SceAppContentMountPoint* mountPoint) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentTemporaryDataFormat(const SceAppContentMountPoint* mountPoint) {
  LOG_USE_MODULE(libSceAppContent);
  auto tempFolder = accessFileManager().getMappedPath(mountPoint->data);
  if (tempFolder) {
    std::filesystem::remove_all(*tempFolder);
  } else {
    LOG_ERR(L"unknown temp folder: %s", tempFolder->c_str());
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentDownloadDataFormat(const SceAppContentMountPoint* mountPoint) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentAddcontDelete(SceNpServiceLabel serviceLabel, const SceNpUnifiedEntitlementLabel* entitlementLabel) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAppContentGetAddcontDownloadProgress(SceNpServiceLabel serviceLabel, const SceNpUnifiedEntitlementLabel* entitlementLabel,
                                                                SceAppContentAddcontDownloadProgress* progress) {
  LOG_USE_MODULE(libSceAppContent);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}