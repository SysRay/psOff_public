#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"

enum class SceMsgDialogMode {
  SCE_MSG_DIALOG_MODE_USER_MSG = 1,
  SCE_MSG_DIALOG_MODE_PROGRESS_BAR,
  SCE_MSG_DIALOG_MODE_SYSTEM_MSG,
};

enum class SceMsgDialogButton : int32_t {
  ID_INVALID = 0,
  ID_OK      = 1,
  ID_YES     = 1,
  ID_NO      = 2,
  ID_BUTTON1 = 1,
  ID_BUTTON2 = 2,
};

enum class SceMsgDialogSystemMessageType {
  TRC_EMPTY_STORE                             = 0,
  TRC_PSN_CHAT_RESTRICTION                    = 1,
  TRC_PSN_UGC_RESTRICTION                     = 2,
  CAMERA_NOT_CONNECTED                        = 4,
  WARNING_PROFILE_PICTURE_AND_NAME_NOT_SHARED = 5,
};

struct SceMsgDialogResult {
  SceMsgDialogMode   mode;
  int32_t            result;
  SceMsgDialogButton buttonId;
  char               pad[32];
};

enum class SceMsgDialogButtonType {
  TYPE_OK                     = 0,
  TYPE_YESNO                  = 1,
  TYPE_NONE                   = 2,
  TYPE_OK_CANCEL              = 3,
  TYPE_WAIT                   = 5,
  TYPE_WAIT_CANCEL            = 6,
  TYPE_YESNO_FOCUS_NO         = 7,
  TYPE_OK_CANCEL_FOCUS_CANCEL = 8,
  TYPE_2BUTTONS               = 9,
};

enum class SceMsgDialogProgressBarType {
  PERCENTAGE,
  PERCENTAGE_CANCEL,
};

struct SceMsgDialogButtonsParam {
  const char* msg1;
  const char* msg2;
  char        pad[32];
};

struct SceMsgDialogUserMessageParam {
  SceMsgDialogButtonType buttonType;
  int32_t : 32;
  const char*               msg;
  SceMsgDialogButtonsParam* buttonsParam;
  char                      pad[24];
};

struct SceMsgDialogProgressBarParam {
  SceMsgDialogProgressBarType barType;
  int32_t : 32;
  const char* msg;
  char        pad[64];
};

struct SceMsgDialogSystemMessageParam {
  SceMsgDialogSystemMessageType sysMsgType;
  char                          pad[32];
};

struct SceMsgDialogParam {
  SceCommonDialogBaseParam baseParam;
  size_t                   size;
  SceMsgDialogMode         mode;
  int32_t : 32;
  SceMsgDialogUserMessageParam*   userMsgParam;
  SceMsgDialogProgressBarParam*   progBarParam;
  SceMsgDialogSystemMessageParam* sysMsgParam;
  SceUserServiceUserId            userId;
  char                            pad[40];
  int32_t : 32;
};
