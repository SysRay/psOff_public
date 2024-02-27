#pragma once
#include <stdint.h>

namespace Err {} // namespace Err

constexpr int32_t SCE_SAVE_DATA_DIALOG_USER_MSG_MAXSIZE = 256;

enum class SceSaveDataDialogMode : int32_t {
  INVALID        = 0,
  LIST           = 1,
  USER_MSG       = 2,
  SYSTEM_MSG     = 3,
  ERROR_CODE     = 4,
  PROGRESS_BAR   = 5,
  WIZARD_LIST    = 6,
  WIZARD_CONFIRM = 7

};

enum class SceSaveDataDialogType : int32_t {
  INVALID = 0,
  SAVE    = 1,
  LOAD    = 2,
  DELETE  = 3

};

enum class SceSaveDataDialogFocusPos : int32_t {
  LISTHEAD   = 0,
  LISTTAIL   = 1,
  DATAHEAD   = 2,
  DATATAIL   = 3,
  DATALATEST = 4,
  DATAOLDEST = 5,
  DIRNAME    = 6

};

enum class SceSaveDataDialogSystemMessageType : int32_t {
  INVALID               = 0,
  NODATA                = 1,
  CONFIRM               = 2,
  OVERWRITE             = 3,
  NOSPACE               = 4,
  PROGRESS              = 5,
  FILE_CORRUPTED        = 6,
  FINISHED              = 7,
  NOSPACE_CONTINUABLE   = 8,
  CORRUPTED_AND_DELETE  = 10,
  CORRUPTED_AND_CREATE  = 11,
  CORRUPTED_AND_RESTORE = 13,
  TOTAL_SIZE_EXCEEDED   = 14,
  NOSPACE_RESTORE       = 15

};

enum class SceSaveDataDialogWizardOption : int32_t {
  RESTORE_OR_DELETE        = 0x1,
  RESTORE_OR_RECREATE      = 0x2,
  RESTORE_OR_CORRUPTED     = 0x3,
  SELECT_WITH_CONFIRMATION = 0x10000,
  DISPLAY_NO_DATA          = 0x20000
};

enum class SceSaveDataDialogButtonType : int32_t { OK, YESNO, NONE, OKCANCEL };

enum class SceSaveDataDialogButtonId : int32_t { INVALID = 0, OK = 1, YES = 1, NO = 2 };

enum class SceSaveDataDialogOptionBack : int32_t { ENABLE = 0, DISABLE = 1 };

enum class SceSaveDataDialogProgressBarType : int32_t { PERCENTAGE = 0 };

enum class SceSaveDataDialogProgressBarTarget : int32_t { BAR_DEFAULT = 0 };

enum class SceSaveDataDialogItemStyle : int32_t { TITLE_DATESIZE_SUBTITLE, TITLE_SUBTITLE_DATESIZE, TITLE_DATESIZE };

enum class SceSaveDataDialogAnimation : int32_t { ON = 0, OFF = 1 };

enum class SceSaveDataDialogUserMessageType : int32_t { NORMAL = 0, ERROR = 1 };

enum class SceSaveDataDialogProgressSystemMessageType : int32_t { INVALID = 0, PROGRESS = 1, RESTORE = 2 };