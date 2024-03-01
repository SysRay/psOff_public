#include "../libSceNpManager/types.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpTrophy);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpTrophy";

EXPORT SYSV_ABI int sceNpTrophyCreateHandle(SceNpTrophyHandle* handle) {
  *handle = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyDestroyHandle(SceNpTrophyHandle handle) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyAbortHandle(SceNpTrophyHandle handle) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyCreateContext(SceNpTrophyContext* context, int32_t userId, SceNpServiceLabel serviceLabel, uint64_t options) {
  *context = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyDestroyContext(SceNpTrophyContext context) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyRegisterContext(SceNpTrophyContext context, SceNpTrophyHandle handle, uint64_t options) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyUnlockTrophy(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyId trophyId, SceNpTrophyId* platinumId) {
  *platinumId = Err::SCE_NP_TROPHY_INVALID_TROPHY_ID;
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyUnlockState(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyFlagArray* flags, uint32_t* count) {
  flags->flagBits[0] = 0;
  flags->flagBits[1] = 0;
  flags->flagBits[2] = 0;
  flags->flagBits[3] = 0;

  *count = 2;

  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetGameInfo(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyGameDetails* details, SceNpTrophyGameData* data) {
  details->numGroups   = 0;
  details->numTrophies = 1;
  details->numPlatinum = 0;
  details->numGold     = 0;
  details->numSilver   = 0;
  details->numBronze   = 1;

  strcpy_s(details->title, "tropyName");
  strcpy_s(details->description, "tropyDesc");

  if (data != nullptr) {
    data->unlockedTrophies   = 0;
    data->unlockedPlatinum   = 0;
    data->unlockedGold       = 0;
    data->unlockedSilver     = 0;
    data->unlockedBronze     = 0;
    data->progressPercentage = 0;
  }
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetGroupInfo(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyGroupId groupId, SceNpTrophyGroupDetails* details,
                                            SceNpTrophyGroupData* data) {
  if (details != nullptr) {
    details->groupId     = groupId;
    details->numTrophies = 0;
    details->numPlatinum = 0;
    details->numGold     = 0;
    details->numSilver   = 0;
    details->numBronze   = 0;

    strcpy_s(details->title, "groupName");
    strcpy_s(details->description, "groupDesc");
  }

  if (data != nullptr) {
    data->groupId            = groupId;
    data->unlockedTrophies   = 0;
    data->unlockedPlatinum   = 0;
    data->unlockedGold       = 0;
    data->unlockedSilver     = 0;
    data->unlockedBronze     = 0;
    data->progressPercentage = 0;
  }
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyInfo(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyId trophyId, SceNpTrophyDetails* details,
                                             SceNpTrophyData* data) {
  if (details != nullptr) {
    details->trophyId    = trophyId;
    details->trophyGrade = SceNpTrophyGrade::GRADE_BRONZE;
    details->groupId     = -1;
    details->hidden      = false;

    strcpy_s(details->name, "tropyName");
    strcpy_s(details->description, "tropyDesc");
  }

  if (data != nullptr) {
    data->trophyId  = trophyId;
    data->unlocked  = false;
    data->timestamp = 0;
  }
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetGameIcon(SceNpTrophyContext context, SceNpTrophyHandle handle, void* buffer, size_t* size) {
  *size              = 8;
  *(uint64_t*)buffer = 0;

  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetGroupIcon(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyGroupId groupId, void* buffer, size_t* size) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyIcon(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyId trophyId, void* buffer, size_t* size) {
  *size              = 8;
  *(uint64_t*)buffer = 0;
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyShowTrophyList(SceNpTrophyContext context, SceNpTrophyHandle handle) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyCaptureScreenshot(SceNpTrophyHandle handle, const SceNpTrophyScreenshotTarget* targets, uint32_t numTargets) {
  return Ok;
}
}