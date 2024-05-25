#include "../libSceNpManager/types.h"
#include "common.h"
#include "core/fileManager/fileManager.h"
#include "core/trophies/trophies.h"
#include "logging.h"
#include "types.h"

#include <fstream>

LOG_DEFINE_MODULE(libSceNpTrophy);

namespace {
static int32_t searchForPng(SceNpTrophyContext context, const char* name, void* buffer, size_t* size) {
  if (auto ctxp = accessTrophies().getContext(context)) {
    int32_t errcode = Err::NpTrophy::ICON_FILE_NOT_FOUND;

    ITrophies::trp_context tctx = {
        .pngim =
            {
                .func = [&errcode, name, buffer, size](ITrophies::trp_png_cb::data_t* data) -> bool {
                  if (data->pngname == name) {
                    if (data->pngsize <= *size) {
                      ::memcpy(buffer, data->pngdata, data->pngsize);
                      *size   = data->pngsize;
                      errcode = Ok;
                    } else {
                      LOG_USE_MODULE(libSceNpTrophy);
                      LOG_ERR(L"Specified buffer is insufficient to save a PNG image (g: %llu, e: %llu)!", *size, data->pngsize);
                      errcode = Err::NpTrophy::INSUFFICIENT_BUFFER;
                      *size   = 0;
                    }
                    ::free(data->pngdata);
                    return true;
                  }

                  return false;
                },
            },
    };

    if (accessTrophies().parseTRP(ctxp->label, &tctx) != ITrophies::ErrCodes::SUCCESS) errcode = Err::NpTrophy::BROKEN_DATA;
    return errcode;
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpTrophy";

//  Unused handles API

EXPORT SYSV_ABI int sceNpTrophyCreateHandle(SceNpTrophyHandle* handle) {
  *handle = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyDestroyHandle(SceNpTrophyHandle) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyAbortHandle(SceNpTrophyHandle) {
  return Ok;
}

//- Unused handles API

EXPORT SYSV_ABI int sceNpTrophyCreateContext(SceNpTrophyContext* context, int32_t userId, SceNpServiceLabel serviceLabel, uint64_t options) {
  return accessTrophies().createContext(userId, (uint32_t)serviceLabel, context);
}

EXPORT SYSV_ABI int sceNpTrophyDestroyContext(SceNpTrophyContext context) {
  if (auto ctxp = accessTrophies().getContext(context)) {
    return accessTrophies().destroyContext(ctxp);
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}

// We don't need any registration
EXPORT SYSV_ABI int sceNpTrophyRegisterContext(SceNpTrophyContext context, SceNpTrophyHandle, uint64_t options) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyUnlockTrophy(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyId trophyId, SceNpTrophyId* platinumId) {
  if (auto ctxp = accessTrophies().getContext(context)) {
    return accessTrophies().unlockTrophy(ctxp, trophyId, platinumId);
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyUnlockState(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyFlagArray* flags, uint32_t* count) {
  if (auto ctxp = accessTrophies().getContext(context)) {
    return accessTrophies().getProgress(ctxp, flags->flagBits, count);
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}

EXPORT SYSV_ABI int sceNpTrophyGetGameInfo(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyGameDetails* details, SceNpTrophyGameData* gdata) {
  if (details == nullptr && gdata == nullptr) return Err::NpTrophy::INVALID_ARGUMENT;
  LOG_USE_MODULE(libSceNpTrophy);

  if (auto ctxp = accessTrophies().getContext(context)) {
    SceNpTrophyFlagArray unlock_progr = {0};
    uint32_t             unlock_count = 0; // Total unlocked trophies count
    uint32_t             trophy_count = 0; // Total trophies count

    int32_t errcode;
    if ((errcode = accessTrophies().getProgress(ctxp, unlock_progr.flagBits, nullptr)) != Ok) return errcode;
    errcode = Err::NpTrophy::INVALID_NP_TITLE_ID;

    ITrophies::trp_context tctx = {
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
                .func = [&errcode, details, &trophy_count](ITrophies::trp_inf_cb::data_t* data) -> bool {
                  if (details) data->title_name.copy(details->title, sizeof(details->title));
                  if (details) data->title_detail.copy(details->description, sizeof(details->description));
                  if (details) details->numTrophies = data->trophy_count;
                  if (details) details->numGroups = data->group_count;
                  trophy_count = data->trophy_count;
                  errcode      = Ok;
                  return true;
                },
            },
    };

    ITrophies::ErrCodes ec;
    if ((ec = accessTrophies().parseTRP(ctxp->label, &tctx)) != ITrophies::ErrCodes::SUCCESS) {
      LOG_ERR(L"Failed to parse trophy data: %S", accessTrophies().getError(ec));
      return ec == ITrophies::ErrCodes::MAX_TROPHY_REACHED ? Err::NpTrophy::EXCEEDS_MAX : errcode;
    }

    if (gdata) gdata->progressPercentage = (unlock_count / (float)trophy_count) * 100;

    return errcode;
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}

EXPORT SYSV_ABI int sceNpTrophyGetGroupInfo(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyGroupId groupId, SceNpTrophyGroupDetails* details,
                                            SceNpTrophyGroupData* grdata) {
  if (grdata == nullptr && details == nullptr) return Err::NpTrophy::INVALID_ARGUMENT;
  LOG_USE_MODULE(libSceNpTrophy);

  if (auto ctxp = accessTrophies().getContext(context)) {
    SceNpTrophyFlagArray unlock_progr = {0};
    uint32_t             unlock_count = 0; // Unlocked trophies in specified group
    uint32_t             trophy_count = 0; // Trophies in specified group

    int32_t errcode;
    if ((errcode = accessTrophies().getProgress(ctxp, unlock_progr.flagBits, nullptr)) != Ok) return errcode;
    errcode = Err::NpTrophy::INVALID_GROUP_ID;

    ITrophies::trp_context tctx = {
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

                  return false;
                },
            },
        .group =
            {
                .func = [&errcode, groupId, details, grdata](ITrophies::trp_grp_cb::data_t* data) -> bool {
                  if (data->id == groupId) {
                    if (details != nullptr) {
                      details->groupId = groupId;
                      data->name.copy(details->title, sizeof(details->title));
                      data->detail.copy(details->description, sizeof(details->description));
                    }
                    if (grdata != nullptr) {
                      grdata->groupId = groupId;
                    }

                    errcode = Ok;
                    return true; // We found our group, the rest of data is useless
                  }

                  return false; // Waiting for the next group
                },
            },
    };

    ITrophies::ErrCodes ec;
    if ((ec = accessTrophies().parseTRP(ctxp->label, &tctx)) != ITrophies::ErrCodes::SUCCESS) {
      LOG_ERR(L"Failed to parse trophy data: %S", accessTrophies().getError(ec));
      return Err::NpTrophy::BROKEN_DATA;
    }

    if (grdata) grdata->progressPercentage = (unlock_count / (float)trophy_count) * 100;

    return Ok;
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyInfo(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyId trophyId, SceNpTrophyDetails* details,
                                             SceNpTrophyData* tdata) {
  LOG_USE_MODULE(libSceNpTrophy);

  if (auto ctxp = accessTrophies().getContext(context)) {
    SceNpTrophyFlagArray unlock_progr = {0};

    int32_t errcode;
    if ((errcode = accessTrophies().getProgress(ctxp, unlock_progr.flagBits, nullptr)) != Ok) return errcode;
    errcode = Err::NpTrophy::INVALID_TROPHY_ID;

    ITrophies::trp_context tctx = {
        .lightweight = false,

        .entry =
            {
                .func = [&errcode, ctxp, trophyId, details, tdata, unlock_progr](ITrophies::trp_ent_cb::data_t* data) -> bool {
                  if (data->id == trophyId) {
                    bool unlocked = SCE_NP_TROPHY_FLAG_ISSET(data->id, (&unlock_progr));

                    if (details != nullptr) {
                      details->trophyId = trophyId;
                      details->groupId  = data->group;
                      details->hidden   = data->hidden;
                      data->name.copy(details->name, sizeof(details->name));
                      data->detail.copy(details->description, sizeof(details->description));

                      switch (data->grade) {
                        case 'b': // Bronze trophy
                          details->trophyGrade = SceNpTrophyGrade::BRONZE;
                          break;
                        case 's': // Silver trophy
                          details->trophyGrade = SceNpTrophyGrade::SILVER;
                          break;
                        case 'g': // Gold trophy
                          details->trophyGrade = SceNpTrophyGrade::GOLD;
                          break;
                        case 'p': // Platinum trophy
                          details->trophyGrade = SceNpTrophyGrade::PLATINUM;
                          break;
                      }
                    }

                    if (tdata != nullptr) {
                      tdata->trophyId  = trophyId;
                      tdata->timestamp = (tdata->unlocked = unlocked) ? accessTrophies().getUnlockTime(ctxp, trophyId) : 0ull;
                    }

                    errcode = Ok;
                    return true; // We found our trophy, the rest of data is useless
                  }

                  return false;
                },
            },
    };

    ITrophies::ErrCodes ec;
    if ((ec = accessTrophies().parseTRP(ctxp->label, &tctx)) != ITrophies::ErrCodes::SUCCESS) {
      LOG_ERR(L"Failed to parse trophy data: %S", accessTrophies().getError(ec));
      return ec == ITrophies::ErrCodes::MAX_TROPHY_REACHED ? Err::NpTrophy::EXCEEDS_MAX : Err::NpTrophy::BROKEN_DATA;
    }

    return errcode;
  }

  return Err::NpTrophy::INVALID_CONTEXT;
}

EXPORT SYSV_ABI int sceNpTrophyGetGameIcon(SceNpTrophyContext context, SceNpTrophyHandle, void* buffer, size_t* size) {
  return searchForPng(context, "ICON0.PNG", buffer, size);
}

EXPORT SYSV_ABI int sceNpTrophyGetGroupIcon(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyGroupId groupId, void* buffer, size_t* size) {
  auto name = std::format("GR{:3}.PNG", groupId);
  return searchForPng(context, name.c_str(), buffer, size);
}

EXPORT SYSV_ABI int sceNpTrophyGetTrophyIcon(SceNpTrophyContext context, SceNpTrophyHandle, SceNpTrophyId trophyId, void* buffer, size_t* size) {
  auto name = std::format("TROP{:3}.PNG", trophyId);
  return searchForPng(context, name.c_str(), buffer, size);
}

EXPORT SYSV_ABI int sceNpTrophyShowTrophyList(SceNpTrophyContext context, SceNpTrophyHandle) {
  return Ok;
}

EXPORT SYSV_ABI int sceNpTrophyCaptureScreenshot(SceNpTrophyHandle handle, const SceNpTrophyScreenshotTarget* targets, uint32_t numTargets) {
  return Err::NpTrophy::SCREENSHOT_DISABLED;
}
}
