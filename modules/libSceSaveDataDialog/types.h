#pragma once
#include "../libSceSaveData/types.h"
#include "..\libSceCommonDialog\types.h"
#include "codes.h"


struct SceSaveDataDialogAnimationParam {
  SceSaveDataDialogAnimation userOK;       ///  animation setting when user decide OK or ListItem
  SceSaveDataDialogAnimation userCancel;   ///  animation setting when user cancel
  uint8_t                    reserved[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogNewItem {
  const char* title;        /// title
  void*       iconBuf;      /// icon buffer
  size_t      iconSize;     /// icon size
  uint8_t     reserved[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogItems {
  int32_t userId; /// user id
  int32_t : 32;

  const SceSaveDataTitleId* titleId; /// title id
  const SceSaveDataDirName* dirName; /// savedata directory name
  uint32_t                  dirNameNum;

  int32_t : 32;

  const SceSaveDataDialogNewItem* newItem;  /// settings for new savedata
  SceSaveDataDialogFocusPos       focusPos; /// Initial focus position
  int32_t : 32;

  const SceSaveDataDirName* focusPosDirName;
  /// Initial focus position for DirName
  SceSaveDataDialogItemStyle itemStyle; /// Style of list item appearance

  uint8_t reserved[36]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogUserMessageParam {
  SceSaveDataDialogButtonType      buttonType;   /// Button type
  SceSaveDataDialogUserMessageType msgType;      /// type of user message dialog
  const char*                      msg;          /// Displayed message strings
  uint8_t                          reserved[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogSystemMessageParam {
  SceSaveDataDialogSystemMessageType sysMsgType; /// System message type
  int32_t : 32;

  uint64_t value;        /// Additional value
  uint8_t  reserved[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogErrorCodeParam {
  int32_t errorCode;    /// Displayed error code
  uint8_t reserved[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogProgressBarParam {
  SceSaveDataDialogProgressBarType barType; /// Progress bar type
  int32_t : 32;

  const char*                                msg;          /// Displayed user message strings
  SceSaveDataDialogProgressSystemMessageType sysMsgType;   /// System message type
  uint8_t                                    reserved[28]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogWizardParam {

  SceSaveDataDialogWizardOption option; /// Wizard behavior option

  int32_t reserved1; /// Reserved member (must be set to zero)

  const SceSaveDataFingerprint* fingerprint; /// fingerprint

  uint8_t reserved2[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogOptionParam {
  SceSaveDataDialogOptionBack back;

  uint8_t reserved[32]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogParam {
  SceCommonDialogBaseParam baseParam; /// Common parameter
  int32_t                  size;      /// Structure size (must be initialized by using sceMsgDialogParamInit)

  SceSaveDataDialogMode mode;     /// Mode of calling function
  SceSaveDataDialogType dispType; /// Displayed string type
  int32_t : 32;

  SceSaveDataDialogAnimationParam* animParam; /// animation setting
  SceSaveDataDialogItems*          items;     /// Parameter for list/other mode item savedata

  SceSaveDataDialogUserMessageParam*   userMsgParam;   /// Parameter for user message
  SceSaveDataDialogSystemMessageParam* sysMsgParam;    /// Parameter for system prepared message
  SceSaveDataDialogErrorCodeParam*     errorCodeParam; /// Parameter for display error code
  SceSaveDataDialogProgressBarParam*   progBarParam;   /// Parameter for progress bar

  void* userData; /// User data can be received at SceSaveDataDialogResult

  SceSaveDataDialogOptionParam* optionParam;

  SceSaveDataDialogWizardParam* wizardParam; /// Parameter for wizard behavior settings

  uint8_t reserved[16]; /// Reserved range (must be filled by zero)
};

struct SceSaveDataDialogCloseParam {
  SceSaveDataDialogAnimation anim; /// animation setting when dialog closing
  uint8_t                    reserved[32];
};

struct SceSaveDataDialogResult {
  SceSaveDataDialogMode     mode;     /// Mode of function
  int32_t                   result;   /// Result of executing function
  SceSaveDataDialogButtonId buttonId; /// Id of button user selected
  int32_t : 32;

  SceSaveDataDirName* dirName; /// savedata directory name

  SceSaveDataParam* param; /// Buffer to receive savedata information ( can be set NULL if you don't need it)

  void*   userData;     /// Userdata specified at calling function
  uint8_t reserved[32]; /// Reserved range (must be filled by zero)
};