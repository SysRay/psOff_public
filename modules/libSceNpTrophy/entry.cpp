#include "../libSceNpManager/types.h"
#include "common.h"
#include "core/fileManager/fileManager.h"
#include "core/trophies/trophies.h"
#include "logging.h"
#include "types.h"

#include <fstream>

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
  return accessTrophies().createContext(userId, (uint32_t)serviceLabel) ? Ok : Err::NpTrophy::ALREADY_EXISTS;
}

EXPORT SYSV_ABI int sceNpTrophyDestroyContext(SceNpTrophyContext context) {
  return accessTrophies().destroyContext(context) ? Ok : Err::NpTrophy::INVALID_ARGUMENT;
}

EXPORT SYSV_ABI int sceNpTrophyRegisterContext(SceNpTrophyContext context, SceNpTrophyHandle handle, uint64_t options) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyUnlockTrophy(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyId trophyId, SceNpTrophyId* platinumId) {
  *platinumId = NpTrophy::INVALID_TROPHY_ID;
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyUnlockState(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyFlagArray* flags, uint32_t* count) {
  return accessTrophies().getProgress(context, flags->flagBits, count);
}

EXPORT SYSV_ABI int sceNpTrophyGetGameInfo(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyGameDetails* details, SceNpTrophyGameData* gdata) {
  LOG_USE_MODULE(libSceNpTrophy);
  SceNpTrophyFlagArray unlock_progr = {0};
  uint32_t             unlock_count = 0;
  uint32_t             trophy_count = 0;

  if (!accessTrophies().getProgress(context, unlock_progr.flagBits, &unlock_count)) return Err::NpTrophy::INVALID_CONTEXT;

  ITrophies::trp_context ctx = {
      .lightweight = false,

      .entry =
          {
              .func = [details, gdata, unlock_progr](ITrophies::trp_ent_cb::data_t* data) -> bool {
                bool unlocked = SCE_NP_TROPHY_FLAG_ISSET(data->id, (&unlock_progr));

                switch (data->grade) {
                  case 'b': // Bronze trophy
                    if (details) ++details->numBronze;
                    if (unlocked && gdata) ++gdata->unlockedBronze;
                    break;
                  case 's': // Silver trophy
                    if (details) ++details->numSilver;
                    if (unlocked && gdata) ++gdata->unlockedSilver;
                    break;
                  case 'g': // Gold trophy
                    if (details) ++details->numGold;
                    if (unlocked && gdata) ++gdata->unlockedGold;
                    break;
                  case 'p': // Platinum trophy
                    if (details) ++details->numPlatinum;
                    if (unlocked && gdata) ++gdata->unlockedPlatinum;
                    break;
                }

                return false;
              },
          },
      .itrop =
          {
              .func = [details, &trophy_count](ITrophies::trp_inf_cb::data_t* data) -> bool {
                if (details) data->title_name.copy(details->title, sizeof(details->title));
                if (details) data->title_detail.copy(details->description, sizeof(details->description));
                if (details) details->numTrophies = data->trophy_count;
                if (details) details->numGroups = data->group_count;
                trophy_count = data->trophy_count;
                return true;
              },
          },
  };

  ITrophies::ErrCodes ec;
  if ((ec = accessTrophies().parseTRP(&ctx)) != ITrophies::ErrCodes::SUCCESS) {
    LOG_ERR(L"Failed to parse trophy data: %S", accessTrophies().getError(ec));
    return ec == ITrophies::ErrCodes::MAX_TROPHY_REACHED ? Err::NpTrophy::EXCEEDS_MAX : Err::NpTrophy::INVALID_ARGUMENT;
  }

  if (gdata) gdata->progressPercentage = (unlock_count / (float)trophy_count) * 100;

  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetGroupInfo(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyGroupId groupId, SceNpTrophyGroupDetails* details,
                                            SceNpTrophyGroupData* grdata) {
  LOG_USE_MODULE(libSceNpTrophy);
  SceNpTrophyFlagArray unlock_progr = {0};
  uint32_t             unlock_count = 0;
  uint32_t             trophy_count = 0;

  if (!accessTrophies().getProgress(context, unlock_progr.flagBits, nullptr)) return Err::NpTrophy::INVALID_CONTEXT;

  ITrophies::trp_context ctx = {
      .lightweight = false,

      .entry =
          {
              .func = [groupId, details, grdata, unlock_progr, &trophy_count, &unlock_count](ITrophies::trp_ent_cb::data_t* data) -> bool {
                if (data->group == groupId) {
                  bool unlocked = SCE_NP_TROPHY_FLAG_ISSET(data->id, (&unlock_progr));

                  switch (data->grade) {
                    case 'b': // Bronze trophy
                      if (details) ++details->numBronze;
                      if (unlocked && grdata) ++grdata->unlockedBronze;
                      break;
                    case 's': // Silver trophy
                      if (details) ++details->numSilver;
                      if (unlocked && grdata) ++grdata->unlockedSilver;
                      break;
                    case 'g': // Gold trophy
                      if (details) ++details->numGold;
                      if (unlocked && grdata) ++grdata->unlockedGold;
                      break;
                    case 'p': // Platinum trophy
                      if (details) ++details->numPlatinum;
                      if (unlocked && grdata) ++grdata->unlockedPlatinum;
                      break;
                  }

                  if (details) ++details->numTrophies;
                  if (unlocked && grdata) ++grdata->unlockedTrophies;
                  if (unlocked) ++unlock_count;
                  ++trophy_count;
                }

                return false; // todo: Find trophies of specific group
              },
          },
      .group =
          {
              .func = [groupId, details, grdata](ITrophies::trp_grp_cb::data_t* data) -> bool {
                if (data->id == groupId) {
                  if (details != nullptr) {
                    details->groupId = groupId;
                    data->name.copy(details->title, sizeof(details->title));
                    data->detail.copy(details->description, sizeof(details->description));
                  }
                  if (grdata != nullptr) {
                    grdata->groupId = groupId;
                  }
                  return true; // We found our group, the rest of data is useless
                }

                return false; // Waiting for the next group
              },
          },
  };

  ITrophies::ErrCodes ec;
  if ((ec = accessTrophies().parseTRP(&ctx)) != ITrophies::ErrCodes::SUCCESS) {
    LOG_ERR(L"Failed to parse trophy data: %S", accessTrophies().getError(ec));
    return ec == ITrophies::ErrCodes::MAX_TROPHY_REACHED ? Err::NpTrophy::EXCEEDS_MAX : Err::NpTrophy::INVALID_ARGUMENT;
  }

  if (grdata) grdata->progressPercentage = (unlock_count / (float)trophy_count) * 100;

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
  ITrophies::trp_context ctx = {
      .pngim =
          {
              .func = [buffer, size](ITrophies::trp_png_cb::data_t* data) -> bool {
                if (data->pngname == "ICON0.PNG") {
                  if (data->pngsize <= *size) {
                    ::memcpy(buffer, data->pngdata, data->pngsize);
                    *size = data->pngsize;
                  } else {
                    LOG_USE_MODULE(libSceNpTrophy);
                    LOG_ERR(L"Specified buffer is insufficient to save a PNG image (g: %llu, e: %llu)!", *size, data->pngsize);
                    *size = 0;
                  }
                  ::free(data->pngdata);
                  return true;
                }

                return false;
              },
          },
  };

  return accessTrophies().parseTRP(&ctx) == ITrophies::ErrCodes::SUCCESS;
}

EXPORT SYSV_ABI int sceNpTrophyGetGroupIcon(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyGroupId groupId, void* buffer, size_t* size) {
  *size              = 8;
  *(uint64_t*)buffer = 0;

  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyIcon(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyId trophyId, void* buffer, size_t* size) {
  auto name = std::format("TROP{:3}.PNG", trophyId);

  ITrophies::trp_context ctx = {
      .pngim =
          {
              .func = [name, buffer, size](ITrophies::trp_png_cb::data_t* data) -> bool {
                if (data->pngname == name) {
                  if (data->pngsize <= *size) {
                    ::memcpy(buffer, data->pngdata, data->pngsize);
                    *size = data->pngsize;
                  } else {
                    LOG_USE_MODULE(libSceNpTrophy);
                    LOG_ERR(L"Specified buffer is insufficient to save a PNG image (g: %llu, e: %llu)!", *size, data->pngsize);
                    *size = 0;
                  }
                  ::free(data->pngdata);
                  return true;
                }

                return false;
              },
          },
  };

  return accessTrophies().parseTRP(&ctx) == ITrophies::ErrCodes::SUCCESS;
}

EXPORT SYSV_ABI int sceNpTrophyShowTrophyList(SceNpTrophyContext context, SceNpTrophyHandle handle) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyCaptureScreenshot(SceNpTrophyHandle handle, const SceNpTrophyScreenshotTarget* targets, uint32_t numTargets) {
  return Ok;
}
}
