#pragma once
#include "..\libSceCommonDialog\types.h"
#include "..\libSceIme\types.h"
#include "codes.h"

enum class SceImeDialogEndStatus {
  OK,
  USER_CANCELED,
  ABORTED,
};

enum class SceImeDialogStatus { NONE = 0, RUNNING = 1, FINISHED = 2 };

struct SceImeDialogParam {
  SceUserServiceUserId      userId;
  SceImeType                type;
  uint64_t                  supportedLanguages;
  SceImeEnterLabel          enterLabel;
  SceImeInputMethod         inputMethod;
  SceImeTextFilter          filter;
  uint32_t                  option;
  uint32_t                  maxTextLength;
  wchar_t*                  inputTextBuffer;
  float                     posx;
  float                     posy;
  SceImeHorizontalAlignment horizontalAlignment;
  SceImeVerticalAlignment   verticalAlignment;
  const wchar_t*            placeholder;
  const wchar_t*            title;
  int8_t                    reserved[16];
};

struct SceImeDialogResult {
  SceImeDialogEndStatus endstatus;
  int8_t                reserved[12];
};
