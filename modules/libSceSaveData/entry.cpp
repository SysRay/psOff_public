#include "codes.h"
#include "common.h"
#include "core/fileManager/fileManager.h"
#include "core/kernel/filesystem.h"
#include "logging.h"
#include "types.h"

#include <mutex>

LOG_DEFINE_MODULE(libSceSaveData);

namespace {

struct PImpl {
  PImpl() = default;
  std::mutex mutex;

  std::array<std::string, SCE_SAVE_DATA_NUM_MOUNTS> mounts;
};

PImpl* getData() {
  static PImpl data;
  return &data;
}

int saveDataMount(int32_t userId, const char* dirName, SceSaveDataMountMode mountMode, SceSaveDataMountResult* mountResult) {
  LOG_USE_MODULE(libSceSaveData);

  auto saveDir = std::string(dirName);
  std::transform(saveDir.begin(), saveDir.end(), saveDir.begin(), ::tolower);
  auto dirSaveFiles = accessFileManager().getGameFilesDir() / SAVE_DATA_POINT.substr(1) / saveDir;

  mountResult->mountStatus = 0;

  bool doCreate = ((uint32_t)mountMode & ((uint32_t)SceSaveDataMountMode::CREATE) | (uint32_t)SceSaveDataMountMode::CREATE2) > 0;
  bool doOpen   = ((uint32_t)mountMode & ((uint32_t)SceSaveDataMountMode::READ_ONLY) | (uint32_t)SceSaveDataMountMode::READ_WRITE) > 0;

  if (doOpen || doCreate) {
    if (!accessFileManager().getMountPoint(MountType::Save, dirSaveFiles.filename().string()).empty()) {
      LOG_DEBUG(L"Savedir already created dir:%S", dirName);
      return Err::SAVE_DATA_ERROR_BUSY;
    }
  }

  // Handle create
  if (doCreate) {
    if (!std::filesystem::exists(dirSaveFiles)) {
      std::filesystem::create_directories(dirSaveFiles);
      mountResult->mountStatus = (uint32_t)SceSaveDataMountStatus::CREATED;
    }
  }

  // Handle Open
  if (doOpen) {
    std::string mountPoint;
    // Get Mountpoint
    {
      auto* pimpl = getData();

      std::unique_lock lock(pimpl->mutex);
      for (size_t n = 0; n < pimpl->mounts.size(); ++n) {
        if (pimpl->mounts[n].empty()) {
          mountPoint       = SAVE_DATA_POINT.data() + std::to_string(n);
          pimpl->mounts[n] = mountPoint;
          break;
        }
      }
      if (mountPoint.empty()) return Err::SAVE_DATA_ERROR_MOUNT_FULL;
    }
    // - mountpoint

    accessFileManager().addMountPoint(mountPoint, dirSaveFiles, MountType::Save);

    if (!std::filesystem::exists(dirSaveFiles)) {
      LOG_DEBUG(L"Savedir doesn't exist");
      return Err::SAVE_DATA_ERROR_NOT_FOUND;
    }

    auto const count = mountPoint.copy(mountResult->mountPoint.data, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE - 1);

    mountResult->mountPoint.data[count] = '\0';
    mountResult->requiredBlocks         = 0;
  }

  LOG_INFO(L"userId:%d dirName:%S mountMode:0x%04x status:%u-> path:%s mountPoint:%S", userId, dirName, mountMode, mountResult->mountStatus,
           dirSaveFiles.c_str(), mountResult->mountPoint.data);
  return Ok;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSaveData";

EXPORT SYSV_ABI int32_t sceSaveDataInitialize(const SceSaveDataInitParams3* initParam) {

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataInitialize2(const SceSaveDataInitParams3* initParam) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataInitialize3(const SceSaveDataInitParams3* initParam) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataTerminate() {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataMount(const SceSaveDataMount* mount, SceSaveDataMountResult* mountResult) {
  return saveDataMount(mount->userId, mount->dirName, mount->mountMode, mountResult);
}

EXPORT SYSV_ABI int32_t sceSaveDataMount2(const SceSaveDataMount2* mount, SceSaveDataMountResult* mountResult) {
  return saveDataMount(mount->userId, mount->dirName->data, mount->mountMode, mountResult);
}

EXPORT SYSV_ABI int32_t sceSaveDataTransferringMount(const SceSaveDataTransferringMount* mount, SceSaveDataMountResult* mountResult) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataUmount(const SceSaveDataMountPoint* mountPoint) {
  std::string_view path(&mountPoint->data[0]);

  accessFileManager().clearMountPoint(MountType::Save, path.data());

  auto* pimpl = getData();

  std::unique_lock lock(pimpl->mutex);
  for (size_t n = 0; n < pimpl->mounts.size(); ++n) {
    if (pimpl->mounts[n].substr(SAVE_DATA_POINT.size()) == path.substr(SAVE_DATA_POINT.size())) {
      pimpl->mounts[n].clear();
      break;
    }
  }
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataUmountWithBackup(const SceSaveDataMountPoint* mountPoint) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  // todo: backup
  return sceSaveDataUmount(mountPoint);
}

EXPORT SYSV_ABI int32_t sceSaveDataGetMountInfo(const SceSaveDataMountPoint* mountPoint, SceSaveDataMountInfo* info) {
  info->blocks     = 100000;
  info->freeBlocks = 100000;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDelete(const SceSaveDataDelete* del) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataDirNameSearch(const SceSaveDataDirNameSearchCond* cond, SceSaveDataDirNameSearchResult* result) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataSetParam(const SceSaveDataMountPoint* mountPoint, SceSaveDataParamType paramType, const void* paramBuf,
                                            size_t paramBufSize) {
  LOG_USE_MODULE(libSceSaveData);

  LOG_INFO(L"mountPoint:%S paramType:%u, paramBufSize:0x%08llx", mountPoint->data, paramType, paramBufSize);
  if (paramType == SceSaveDataParamType::ALL) {
    const auto* p = static_cast<const SceSaveDataParam*>(paramBuf);

    LOG_INFO(L"title:%S subTile:%S detail:%S userParam:%u", p->title, p->subTitle, p->detail, p->userParam);
  } else {
    LOG_ERR(L"Not implemented");
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataGetParam(const SceSaveDataMountPoint* mountPoint, SceSaveDataParamType paramType, void* paramBuf, size_t paramBufSize,
                                            size_t* gotSize) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataSaveIcon(const SceSaveDataMountPoint* mountPoint, const SceSaveDataIcon* icon) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataLoadIcon(const SceSaveDataMountPoint* mountPoint, SceSaveDataIcon* icon) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataSyncSaveDataMemory(const SceSaveDataMemorySync* syncParam) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataSetupSaveDataMemory2(const SceSaveDataMemorySetup2* setupParam, SceSaveDataMemorySetupResult* result) {
  if (setupParam == nullptr || setupParam->memorySize == 0 || setupParam->option.bits.SET_PARAM == 0) return getErr(ErrCode::_EINVAL);
  auto filename = std::format("SLOT{}_UID{}.dat", setupParam->slotId, setupParam->userId);

  filesystem::SceOpen oflags {.mode = filesystem::SceOpenMode::WRONLY, .create = 1, .excl = 1};

  auto file_handle = filesystem::open(filename.c_str(), oflags, 0);

  if (file_handle > 0) {
    if (filesystem::pwrite(file_handle, "\0", 1, setupParam->memorySize - 1) != 1) {
      filesystem::close(file_handle);
      return getErr(ErrCode::_EIO);
    }

    filesystem::close(file_handle);
  }

  if (result != nullptr) result->existedMemorySize = setupParam->memorySize;

  auto paramname    = filename.substr(0, filename.size() - 3) + "prm";
  auto param_handle = filesystem::open(paramname.c_str(), oflags, 0);

  if (param_handle > 0) {
    if (auto param = setupParam->initParam) {
      if (filesystem::write(param_handle, param, sizeof(*param)) != sizeof(*param)) {
        filesystem::close(param_handle);
        return getErr(ErrCode::_EIO);
      }
    }

    filesystem::close(param_handle);
  }

  auto iconname    = filename.substr(0, filename.size() - 3) + "pic";
  auto icon_handle = filesystem::open(iconname.c_str(), oflags, 0);

  if (icon_handle > 0) {
    if (setupParam->iconMemorySize > 0) {
      if (filesystem::pwrite(icon_handle, "\0", 1, setupParam->iconMemorySize - 1) != 1) {
        filesystem::close(icon_handle);
        return getErr(ErrCode::_EIO);
      }

      if (auto icon = setupParam->initIcon) {
        if (filesystem::pwrite(icon_handle, icon->buf, icon->bufSize, 0) != icon->bufSize) {
          filesystem::close(icon_handle);
          return getErr(ErrCode::_EIO);
        }
      }
    }

    filesystem::close(icon_handle);
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataSetupSaveDataMemory(const SceUserServiceUserId userId, size_t memorySize, const SceSaveDataParam* param) {
  const SceSaveDataMemorySetup2 ssdms2 {
      .option         = SceSaveDataMemoryOption {.bits = {.SET_PARAM = 1}},
      .userId         = userId,
      .memorySize     = memorySize,
      .iconMemorySize = 0,
      .initParam      = param,
      .initIcon       = nullptr,
      .slotId         = 0,
  };

  return sceSaveDataSetupSaveDataMemory2(&ssdms2, nullptr);
}

EXPORT SYSV_ABI int32_t sceSaveDataGetSaveDataMemory2(const SceSaveDataMemoryGet2* getParam) {
  if (getParam == nullptr || getParam->data == nullptr || getParam->data->buf == nullptr) return getErr(ErrCode::_EINVAL);

  auto filename = std::format("SLOT{}_UID{}.dat", getParam->slotId, getParam->userId);

  filesystem::SceOpen oflags {.mode = filesystem::SceOpenMode::RDONLY};

  auto file_handle = filesystem::open(filename.c_str(), oflags, 0);
  if (file_handle < 0) return file_handle; // return the error code returned by filesystem::open

  if (auto data = getParam->data) {
    if (filesystem::pread(file_handle, data->buf, data->bufSize, data->offset) != data->bufSize) {
      filesystem::close(file_handle);
      return getErr(ErrCode::_EIO);
    }
    filesystem::close(file_handle);
  }

  if (auto param = getParam->param) {
    auto paramname    = filename.substr(0, filename.size() - 3) + "prm";
    auto param_handle = filesystem::open(paramname.c_str(), oflags, 0);
    if (param_handle < 0) return param_handle; // return the error code returned by filesystem::open

    if (filesystem::read(param_handle, param, sizeof(*param)) != sizeof(*param)) {
      filesystem::close(param_handle);
      return getErr(ErrCode::_EIO);
    }

    filesystem::close(param_handle);
  }

  if (auto icon = getParam->icon) {
    auto iconname    = filename.substr(0, filename.size() - 3) + "pic";
    auto icon_handle = filesystem::open(iconname.c_str(), oflags, 0);
    if (icon_handle < 0) return icon_handle; // return the error code returned by filesystem::open

    if (filesystem::read(icon_handle, icon->buf, icon->bufSize) != icon->bufSize) {
      filesystem::close(icon_handle);
      return getErr(ErrCode::_EIO);
    }
    filesystem::close(icon_handle);
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataGetSaveDataMemory(const SceUserServiceUserId userId, void* buf, const size_t bufSize, const int64_t offset) {
  if (buf == nullptr || bufSize == 0) return getErr(ErrCode::_EINVAL);
  SceSaveDataMemoryData ssdmd {
      .buf     = buf,
      .bufSize = bufSize,
      .offset  = offset,
  };
  const SceSaveDataMemoryGet2 ssdmg2 {
      .userId = userId,
      .data   = &ssdmd,
      .param  = nullptr,
      .icon   = nullptr,
      .slotId = 0,
  };
  return sceSaveDataGetSaveDataMemory2(&ssdmg2);
}

EXPORT SYSV_ABI int32_t sceSaveDataSetSaveDataMemory2(const SceSaveDataMemorySet2* setParam) {
  if (setParam == nullptr || setParam->data == nullptr || setParam->data->buf == nullptr) return getErr(ErrCode::_EINVAL);

  auto filename = std::format("SLOT{}_UID{}.dat", setParam->slotId, setParam->userId);

  filesystem::SceOpen oflags {.mode = filesystem::SceOpenMode::WRONLY};

  auto file_handle = filesystem::open(filename.c_str(), oflags, 0);
  if (file_handle < 0) return file_handle; // return the error code returned by filesystem::open

  for (uint32_t i = 0; i < setParam->dataNum; ++i) {
    auto& data = setParam->data[i];
    if (data.buf == nullptr) {
      filesystem::close(file_handle);
      return getErr(ErrCode::_EINVAL);
    }
    if (filesystem::pwrite(file_handle, data.buf, data.bufSize, data.offset) != data.bufSize) {
      filesystem::close(file_handle);
      return getErr(ErrCode::_EIO);
    }
  }

  filesystem::close(file_handle);

  if (auto param = setParam->param) {
    auto paramname    = filename.substr(0, filename.size() - 3) + "prm";
    auto param_handle = filesystem::open(paramname.c_str(), oflags, 0);
    if (param_handle < 0) return param_handle; // return the error code returned by filesystem::open

    if (filesystem::write(param_handle, param, sizeof(*param)) != sizeof(*param)) {
      filesystem::close(param_handle);
      return getErr(ErrCode::_EIO);
    }

    filesystem::close(param_handle);
  }

  if (auto icon = setParam->icon) {
    auto iconname    = filename.substr(0, filename.size() - 3) + "pic";
    auto icon_handle = filesystem::open(iconname.c_str(), oflags, 0);
    if (icon_handle < 0) return icon_handle; // return the error code returned by filesystem::open

    if (filesystem::write(icon_handle, icon->buf, icon->bufSize) != icon->bufSize) {
      filesystem::close(icon_handle);
      return getErr(ErrCode::_EIO);
    }

    filesystem::close(icon_handle);
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataSetSaveDataMemory(const SceUserServiceUserId userId, const void* buf, const size_t bufSize, const int64_t offset) {
  SceSaveDataMemoryData ssdmd {
      .buf     = (void*)buf,
      .bufSize = bufSize,
      .offset  = offset,
  };
  const SceSaveDataMemorySet2 ssdms2 {
      .userId  = userId,
      .data    = &ssdmd,
      .param   = nullptr,
      .icon    = nullptr,
      .dataNum = 1,
      .slotId  = 0,
  };
  return sceSaveDataSetSaveDataMemory2(&ssdms2);
}

EXPORT SYSV_ABI int32_t sceSaveDataRestoreBackupData(const SceSaveDataRestoreBackupData* restore) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataCheckBackupData(const SceSaveDataCheckBackupData* check) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataBackup(const SceSaveDataBackup* backup) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataGetProgress(float* progress) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_DEBUG(L"todo %S", __FUNCTION__);
  *progress = 1.0f;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataClearProgress(void) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSaveDataGetEventResult(const SceSaveDataEventParam* eventParam, SceSaveDataEvent* event) {
  LOG_USE_MODULE(libSceSaveData);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
