#pragma once
#include "..\libSceCommonDialog\types.h"
#include "codes.h"

typedef enum SceMsgDialogMode { SCE_MSG_DIALOG_MODE_USER_MSG = 1, SCE_MSG_DIALOG_MODE_PROGRESS_BAR, SCE_MSG_DIALOG_MODE_SYSTEM_MSG } SceMsgDialogMode;

typedef enum SceMsgDialogButton: int32_t {
  SCE_MSG_DIALOG_BUTTON_ID_INVALID = 0,
  SCE_MSG_DIALOG_BUTTON_ID_OK      = 1,
  SCE_MSG_DIALOG_BUTTON_ID_YES     = 1,
  SCE_MSG_DIALOG_BUTTON_ID_NO      = 2,
  SCE_MSG_DIALOG_BUTTON_ID_BUTTON1 = 1,
  SCE_MSG_DIALOG_BUTTON_ID_BUTTON2 = 2,
} SceMsgDialogButton;

typedef enum SceMsgDialogSystemMessageType {
  SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_EMPTY_STORE                             = 0,
  SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_CHAT_RESTRICTION                    = 1,
  SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_UGC_RESTRICTION                     = 2,
  SCE_MSG_DIALOG_SYSMSG_TYPE_CAMERA_NOT_CONNECTED                        = 4,
  SCE_MSG_DIALOG_SYSMSG_TYPE_WARNING_PROFILE_PICTURE_AND_NAME_NOT_SHARED = 5,
} SceMsgDialogSystemMessageType;

typedef struct SceMsgDialogResult {
  SceMsgDialogMode   mode;
  int32_t            result;
  SceMsgDialogButton buttonId;
  char               pad[32];
} SceMsgDialogResult;

typedef enum SceMsgDialogButtonType {
  SCE_MSG_DIALOG_BUTTON_TYPE_OK                     = 0,
  SCE_MSG_DIALOG_BUTTON_TYPE_YESNO                  = 1,
  SCE_MSG_DIALOG_BUTTON_TYPE_NONE                   = 2,
  SCE_MSG_DIALOG_BUTTON_TYPE_OK_CANCEL              = 3,
  SCE_MSG_DIALOG_BUTTON_TYPE_WAIT                   = 5,
  SCE_MSG_DIALOG_BUTTON_TYPE_WAIT_CANCEL            = 6,
  SCE_MSG_DIALOG_BUTTON_TYPE_YESNO_FOCUS_NO         = 7,
  SCE_MSG_DIALOG_BUTTON_TYPE_OK_CANCEL_FOCUS_CANCEL = 8,
  SCE_MSG_DIALOG_BUTTON_TYPE_2BUTTONS               = 9,
} SceMsgDialogButtonType;

typedef enum SceMsgDialogProgressBarType {
  SCE_MSG_DIALOG_PROGRESSBAR_TYPE_PERCENTAGE,
  SCE_MSG_DIALOG_PROGRESSBAR_TYPE_PERCENTAGE_CANCEL,
} SceMsgDialogProgressBarType;

typedef struct SceMsgDialogButtonsParam {
  const char* msg1;
  const char* msg2;
  char        pad[32];
} SceMsgDialogButtonsParam;

typedef struct SceMsgDialogUserMessageParam {
  SceMsgDialogButtonType buttonType;
  int32_t : 32;
  const char*               msg;
  SceMsgDialogButtonsParam* buttonsParam;
  char                      pad[24];
} SceMsgDialogUserMessageParam;

typedef struct SceMsgDialogProgressBarParam {
  SceMsgDialogProgressBarType barType;
  int32_t : 32;
  const char* msg;
  char        pad[64];
} SceMsgDialogProgressBarParam;

typedef struct SceMsgDialogSystemMessageParam {
  SceMsgDialogSystemMessageType sysMsgType;
  char                          pad[32];
} SceMsgDialogSystemMessageParam;

typedef struct SceMsgDialogParam {
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
} SceMsgDialogParam;
