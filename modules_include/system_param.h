#pragma once
#include <stdint.h>

enum class SystemParamLang : uint32_t {
  Japanese           = 0,
  EnglishUS          = 1,
  French             = 2,
  Spanish            = 3,
  German             = 4,
  Italian            = 5,
  Dutch              = 6,
  PortuguesePT       = 7,
  Russian            = 8,
  Korean             = 9,
  ChineseTraditional = 10,
  ChineseSimplified  = 11,
  Finnish            = 12,
  Swedish            = 13,
  Danish             = 14,
  Norwegian          = 15,
  Polish             = 16,
  PortugueseBR       = 17,
  EnglishUK          = 18,
  Turkish            = 19,
  SpanishLA          = 20,
  Arabic             = 21,
  FrenchCA           = 22,
  Czech              = 23,
  Hungarian          = 24,
  Greek              = 25,
  Romanian           = 26,
  Thai               = 27,
  Vietnamese         = 28,
  Indonesian         = 29
};

enum class SystemParamDateFormat : uint32_t { YYYYMMDD = 0, DDMMYYYY = 1, MMDDYYYY = 2 };

enum class SystemParamGameParental : uint32_t {
  OFF      = 0,
  LEVEL_01 = 1,
  LEVEL_02 = 2,
  LEVEL_03 = 3,
  LEVEL_04 = 4,
  LEVEL_05 = 5,
  LEVEL_06 = 6,
  LEVEL_07 = 7,
  LEVEL_08 = 8,
  LEVEL_09 = 9,
  LEVEL_10 = 10,
  LEVEL_11 = 11
};

enum class SystemParamEnterButtonAssign : uint32_t { CIRCLE = 0, CROSS = 1 };