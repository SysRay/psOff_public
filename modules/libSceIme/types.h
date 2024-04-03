#pragma once
#include "codes.h"
#include "common.h"

enum class SCE_IME_MAX {
  PREEDIT_LENGTH          = 30,
  EXPANDED_PREEDIT_LENGTH = 120,
  TEXT_LENGTH             = 2048,
  TEXT_AREA               = 4,
  CANDIDATE_WORD_LENGTH   = 55,
  CANDIDATE_LIST_SIZE     = 100,
};

enum class SCE_IME_LANGUAGE : uint64_t {
  DANISH              = 0x0000000000000001ULL,
  GERMAN              = 0x0000000000000002ULL,
  ENGLISH_US          = 0x0000000000000004ULL,
  SPANISH             = 0x0000000000000008ULL,
  FRENCH              = 0x0000000000000010ULL,
  ITALIAN             = 0x0000000000000020ULL,
  DUTCH               = 0x0000000000000040ULL,
  NORWEGIAN           = 0x0000000000000080ULL,
  POLISH              = 0x0000000000000100ULL,
  PORTUGUESE_PT       = 0x0000000000000200ULL,
  RUSSIAN             = 0x0000000000000400ULL,
  FINNISH             = 0x0000000000000800ULL,
  SWEDISH             = 0x0000000000001000ULL,
  JAPANESE            = 0x0000000000002000ULL,
  KOREAN              = 0x0000000000004000ULL,
  SIMPLIFIED_CHINESE  = 0x0000000000008000ULL,
  TRADITIONAL_CHINESE = 0x0000000000010000ULL,
  PORTUGUESE_BR       = 0x0000000000020000ULL,
  ENGLISH_GB          = 0x0000000000040000ULL,
  TURKISH             = 0x0000000000080000ULL,
  SPANISH_LA          = 0x0000000000100000ULL,
  ARABIC              = 0x0000000001000000ULL,
  FRENCH_CA           = 0x0000000002000000ULL,
  THAI                = 0x0000000004000000ULL,
  CZECH               = 0x0000000008000000ULL,
  GREEK               = 0x0000000010000000ULL,
  INDONESIAN          = 0x0000000020000000ULL,
  VIETNAMESE          = 0x0000000040000000ULL,
  ROMANIAN            = 0x0000000080000000ULL,
  HUNGARIAN           = 0x0000000100000000ULL,
};

enum class SCE_IME_OPTION : uint32_t {
  DEFAULT                            = 0x00000000,
  MULTILINE                          = 0x00000001,
  NO_AUTO_CAPITALIZATION             = 0x00000002,
  PASSWORD                           = 0x00000004,
  LANGUAGES_FORCED                   = 0x00000008,
  EXT_KEYBOARD                       = 0x00000010,
  NO_LEARNING                        = 0x00000020,
  FIXED_POSITION                     = 0x00000040,
  DISABLE_COPY_PASTE                 = 0x00000080,
  DISABLE_RESUME                     = 0x00000100,
  DISABLE_AUTO_SPACE                 = 0x00000200,
  DISABLE_POSITION_ADJUSTMENT        = 0x00000800,
  EXPANDED_PREEDIT_BUFFER            = 0x00001000,
  USE_JAPANESE_EISUU_KEY_AS_CAPSLOCK = 0x00002000,
  USE_OVER_2K_COORDINATES            = 0x00004000,
};

enum class SCE_IME_EXT_OPTION : uint32_t {
  OPTION_DEFAULT                             = 0x00000000,
  OPTION_SET_COLOR                           = 0x00000001,
  OPTION_SET_PRIORITY                        = 0x00000002,
  OPTION_PRIORITY_SHIFT                      = 0x00000004,
  OPTION_PRIORITY_FULL_WIDTH                 = 0x00000008,
  OPTION_PRIORITY_FIXED_PANEL                = 0x00000010,
  OPTION_DISABLE_POINTER                     = 0x00000040,
  OPTION_ENABLE_ADDITIONAL_DICTIONARY        = 0x00000080,
  OPTION_DISABLE_STARTUP_SE                  = 0x00000100,
  OPTION_DISABLE_LIST_FOR_EXT_KEYBOARD       = 0x00000200,
  OPTION_HIDE_KEYPANEL_IF_EXT_KEYBOARD       = 0x00000400,
  OPTION_INIT_EXT_KEYBOARD_MODE              = 0x00000800,
  OPTION_ENABLE_ACCESSIBILITY                = 0x00001000,
  OPTION_ACCESSIBILITY_PANEL_FORCED          = 0x00002000,
  OPTION_ADDITIONAL_DICTIONARY_PRIORITY_MODE = 0x00004000,
};

enum class SceImeDisableDevice : uint32_t {
  DEFAULT      = 0x00000000,
  CONTROLLER   = 0x00000001,
  EXT_KEYBOARD = 0x00000002,
  REMOTE_OSK   = 0x00000004,
};

enum class SceImeInputMethod : uint32_t {
  DEFAULT          = 0,
  STATE_PREEDIT    = 0x01000000,
  STATE_SELECTED   = 0x02000000,
  STATE_NATIVE     = 0x04000000,
  STATE_NATIVE2    = 0x08000000,
  STATE_FULL_WIDTH = 0x10000000,
};

enum class SceImeEventId {
  EVENT_OPEN                      = 0,
  EVENT_UPDATE_TEXT               = 1,
  EVENT_UPDATE_CARET              = 2,
  EVENT_CHANGE_SIZE               = 3,
  EVENT_PRESS_CLOSE               = 4,
  EVENT_PRESS_ENTER               = 5,
  EVENT_ABORT                     = 6,
  EVENT_CANDIDATE_LIST_START      = 7,
  EVENT_CANDIDATE_LIST_END        = 8,
  EVENT_CANDIDATE_WORD            = 9,
  EVENT_CANDIDATE_INDEX           = 10,
  EVENT_CANDIDATE_DONE            = 11,
  EVENT_CANDIDATE_CANCEL          = 12,
  EVENT_CHANGE_DEVICE             = 14,
  EVENT_JUMP_TO_NEXT_OBJECT       = 15,
  EVENT_JUMP_TO_BEFORE_OBJECT     = 16,
  EVENT_CHANGE_WINDOW_TYPE        = 17,
  EVENT_CHANGE_INPUT_METHOD_STATE = 18,
  KEYBOARD_EVENT_OPEN             = 256,
  KEYBOARD_EVENT_KEYCODE_DOWN     = 257,
  KEYBOARD_EVENT_KEYCODE_UP       = 258,
  KEYBOARD_EVENT_KEYCODE_REPEAT   = 259,
  KEYBOARD_EVENT_CONNECTION       = 260,
  KEYBOARD_EVENT_DISCONNECTION    = 261,
  KEYBOARD_EVENT_ABORT            = 262,
};

enum class SceImeHorizontalAlignment {
  LEFT   = 0,
  CENTER = 1,
  RIGHT  = 2,
};

enum class SceImeVerticalAlignment {
  TOP    = 0,
  CENTER = 1,
  BOTTOM = 2,
};

enum class SceImeEnterLabel {
  DEFAULT = 0,
  SEND    = 1,
  SEARCH  = 2,
  GO      = 3,
};

enum class SceImeType {
  DEFAULT     = 0,
  BASIC_LATIN = 1,
  URL         = 2,
  MAIL        = 3,
  NUMBER      = 4,
};

enum class SceImePanelPriority {
  DEFAULT  = 0,
  ALPHABET = 1,
  SYMBOL   = 2,
  ACCENT   = 3,
};

enum class SceImeCaretMovementDirection {
  STILL     = 0,
  LEFT      = 1,
  RIGHT     = 2,
  UP        = 3,
  DOWN      = 4,
  HOME      = 5,
  END       = 6,
  PAGE_UP   = 7,
  PAGE_DOWN = 8,
  TOP       = 9,
  BOTTOM    = 10,
};

enum class SceImeTextAreaMode {
  DISABLE = 0,
  EDIT    = 1,
  PREEDIT = 2,
  SELECT  = 3,
};

enum class sceImeKeyboardState : uint32_t {
  KEYCODE_VALID      = 0x00000001,
  CHARACTER_VALID    = 0x00000002,
  WITH_IME           = 0x00000004,
  FROM_OSK           = 0x00000008,
  FROM_OSK_SHORTCUT  = 0x00000010,
  FROM_IME_OPERATION = 0x00000020,
  REPLACE_CHARACTER  = 0x00000040,
  CONTINUOUS_EVENT   = 0x00000080,
  MODIFIER_L_CTRL    = 0x00000100,
  MODIFIER_L_SHIFT   = 0x00000200,
  MODIFIER_L_ALT     = 0x00000400,
  MODIFIER_L_GUI     = 0x00000800,
  MODIFIER_R_CTRL    = 0x00001000,
  MODIFIER_R_SHIFT   = 0x00002000,
  MODIFIER_R_ALT     = 0x00004000,
  MODIFIER_R_GUI     = 0x00008000,
  LED_NUM_LOCK       = 0x00010000,
  LED_CAPS_LOCK      = 0x00020000,
  LED_SCROLL_LOCK    = 0x00040000,
  RESERVED1          = 0x00080000,
  RESERVED2          = 0x00100000,
  FROM_IME_INPUT     = 0x00200000,
};

enum class sceImeKeyboardOption : uint32_t {
  DEFAULT                        = 0x00000000,
  REPEAT                         = 0x00000001,
  REPEAT_EACH_KEY                = 0x00000002,
  ADD_OSK                        = 0x00000004,
  EFFECTIVE_WITH_IME             = 0x00000008,
  DISABLE_RESUME                 = 0x00000010,
  DISABLE_CAPSLOCK_WITHOUT_SHIFT = 0x00000020,
};

enum class sceImeKeyboardMode : uint32_t {
  AUTO                      = 0x00000000,
  MANUAL                    = 0x00000001,
  ALPHABET                  = 0x00000000,
  NATIVE                    = 0x00000002,
  PART                      = 0x00000004,
  KATAKANA                  = 0x00000008,
  HKANA                     = 0x00000010,
  ARABIC_INDIC_NUMERALS     = 0x00000020,
  DISABLE_FORMAT_CHARACTERS = 0x00000040,
};

enum class sceImeKeyboardResourceId : uint32_t {
  ID_INVALID = 0x00000000,
  ID_OSK     = 0x00000001,
};

enum class SceImeKeyboardStatus { DISCONNECTED = 0, CONNECTED = 1 };

enum class SceImeKeyboardType {
  NONE             = 0,
  DANISH           = 1,
  GERMAN           = 2,
  GERMAN_SW        = 3,
  ENGLISH_US       = 4,
  ENGLISH_GB       = 5,
  SPANISH          = 6,
  SPANISH_LA       = 7,
  FINNISH          = 8,
  FRENCH           = 9,
  FRENCH_BR        = 10,
  FRENCH_CA        = 11,
  FRENCH_SW        = 12,
  ITALIAN          = 13,
  DUTCH            = 14,
  NORWEGIAN        = 15,
  POLISH           = 16,
  PORTUGUESE_BR    = 17,
  PORTUGUESE_PT    = 18,
  RUSSIAN          = 19,
  SWEDISH          = 20,
  TURKISH          = 21,
  JAPANESE_ROMAN   = 22,
  JAPANESE_KANA    = 23,
  KOREAN           = 24,
  SM_CHINESE       = 25,
  TR_CHINESE_ZY    = 26,
  TR_CHINESE_PY_HK = 27,
  TR_CHINESE_PY_TW = 28,
  TR_CHINESE_CG    = 29,
  ARABIC_AR        = 30,
  THAI             = 31,
  CZECH            = 32,
  GREEK            = 33,
  INDONESIAN       = 34,
  VIETNAMESE       = 35,
  ROMANIAN         = 36,
  HUNGARIAN        = 37,
};

enum class SceImeKeyboardDeviceType {
  KEYBOARD = 0,
  OSK      = 1,
};

enum class SceImePanelType {
  HIDE               = 0,
  OSK                = 1,
  DIALOG             = 2,
  CANDIDATE          = 3,
  EDIT               = 4,
  EDIT_AND_CANDIDATE = 5,
  ACCESSIBILITY      = 6,
};

enum class SceImeDeviceType {
  NONE         = 0,
  CONTROLLER   = 1,
  EXT_KEYBOARD = 2,
  REMOTE_OSK   = 3,
};

struct SceImeTextAreaProperty {
  SceImeTextAreaMode mode;
  uint32_t           index;
  int32_t            length;
};

struct SceImeEditText {
  wchar_t*               str;
  uint32_t               caretIndex;
  uint32_t               areaNum;
  SceImeTextAreaProperty textArea[SCE_IME_MAX_TEXT_AREA];
};

struct SceImePositionAndForm {
  SceImePanelType           type;
  float                     posx;
  float                     posy;
  SceImeHorizontalAlignment horizontalAlignment;
  SceImeVerticalAlignment   verticalAlignment;
  uint32_t                  width;
  uint32_t                  height;
};

struct SceImeRect {
  float    x;
  float    y;
  uint32_t width;
  uint32_t height;
};

struct SceImeTextGeometry {
  float    x;
  float    y;
  uint32_t width;
  uint32_t height;
};

struct SceImeCaret {
  float    x;
  float    y;
  uint32_t height;
  uint32_t index;
};

struct SceImeColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct SceImeKeyboardInfo {
  SceUserServiceUserId     userId;
  SceImeKeyboardDeviceType device;
  SceImeKeyboardType       type;
  uint32_t                 repeatDelay;
  uint32_t                 repeatRate;
  SceImeKeyboardStatus     status;
  int8_t                   reserved[12];
};

struct SceImeKeyboardResourceIdArray {
  SceUserServiceUserId userId;
  uint32_t             resourceId[SCE_IME_KEYBOARD_MAX_NUMBER];
};

struct SceImeKeycode {
  uint16_t             keycode;
  wchar_t              character;
  uint32_t             status;
  SceImeKeyboardType   type;
  SceUserServiceUserId userId;
  uint32_t             resourceId;
  SceRtcTick           timestamp;
};

union SceImeEventParam {
  SceImeRect                    rect;
  SceImeEditText                text;
  SceImeCaretMovementDirection  caretMove;
  SceImeKeycode                 keycode;
  SceImeKeyboardResourceIdArray resourceIdArray;
  wchar_t*                      candidateWord;
  int32_t                       candidateIndex;
  SceImeDeviceType              deviceType;
  SceImePanelType               panelType;
  uint32_t                      inputMethodState;
  int8_t                        reserved[64];
};

struct SceImeEvent {
  SceImeEventId    id;
  SceImeEventParam param;
};

typedef void (*SceImeEventHandler)(void* arg, const SceImeEvent* e);
typedef int32_t (*SceImeTextFilter)(wchar_t* outText, uint32_t* outTextLength, const wchar_t* srcText, uint32_t srcTextLength);
typedef int (*SceImeExtKeyboardFilter)(const SceImeKeycode* srcKeycode, uint16_t* outKeycode, uint32_t* outStatus, void* reserved);

struct SceImeParam {
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
  void*                     work;
  void*                     arg;
  SceImeEventHandler        handler;
  int8_t                    reserved[8];
};

struct SceImeParamExtended {
  uint32_t                option;
  SceImeColor             colorBase;
  SceImeColor             colorLine;
  SceImeColor             colorTextField;
  SceImeColor             colorPreedit;
  SceImeColor             colorButtonDefault;
  SceImeColor             colorButtonFunction;
  SceImeColor             colorButtonSymbol;
  SceImeColor             colorText;
  SceImeColor             colorSpecial;
  SceImePanelPriority     priority;
  const char*             additionalDictionaryPath;
  SceImeExtKeyboardFilter extKeyboardFilter;
  uint32_t                disableDevice;
  uint32_t                extKeyboardMode;
  int8_t                  reserved[60];
};

struct SceImeKeyboardParam {
  uint32_t           option;
  int8_t             reserved1[4];
  void*              arg;
  SceImeEventHandler handler;
  int8_t             reserved2[8];
};

struct SceImeKeyboardResourceId {
  SceUserServiceUserId userId;
  uint32_t             resourceId[SCE_IME_KEYBOARD_MAX_NUMBER];
};
