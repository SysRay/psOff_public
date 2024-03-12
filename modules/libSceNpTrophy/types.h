#pragma once
#include "codes.h"

typedef int32_t SceNpTrophyHandle;
typedef int32_t SceNpTrophyContext;
typedef int32_t SceNpTrophyId;
typedef int32_t SceNpTrophyGroupId;

enum class SceNpTrophyGrade : unsigned int {
  GRADE_PLATINUM = 1, // PLATINUM
  GRADE_GOLD     = 2, // GOLD
  GRADE_SILVER   = 3, // SILVER
  GRADE_BRONZE   = 4, // BRONZE

};

typedef uint32_t SceNpTrophyFlagMask;
/* trophy flag array */

constexpr uint32_t SCE_NP_TROPHY_FLAG_SETSIZE    = (128);
constexpr uint32_t SCE_NP_TROPHY_FLAG_BITS       = (sizeof(SceNpTrophyFlagMask) * 8);
constexpr uint32_t SCE_NP_TROPHY_FLAG_BITS_ALL   = ((SceNpTrophyFlagMask)-1);
constexpr uint32_t SCE_NP_TROPHY_FLAG_BITS_SHIFT = (5);
constexpr uint32_t SCE_NP_TROPHY_FLAG_BITS_MASK  = (SCE_NP_TROPHY_FLAG_BITS - 1);
constexpr uint32_t SCE_NP_TROPHY_FLAG_BITS_MAX   = (SCE_NP_TROPHY_FLAG_SETSIZE - 1);

typedef struct SceNpTrophyFlagArray {
  SceNpTrophyFlagMask flagBits[SCE_NP_TROPHY_FLAG_SETSIZE >> SCE_NP_TROPHY_FLAG_BITS_SHIFT];
} SceNpTrophyFlagArray;

#define SCE_NP_TROPHY_FLAG_SET(n, p)   ((p)->flagBits[(n) >> SCE_NP_TROPHY_FLAG_BITS_SHIFT] |= (1 << ((n) & SCE_NP_TROPHY_FLAG_BITS_MASK)))
#define SCE_NP_TROPHY_FLAG_CLR(n, p)   ((p)->flagBits[(n) >> SCE_NP_TROPHY_FLAG_BITS_SHIFT] &= ~(1 << ((n) & SCE_NP_TROPHY_FLAG_BITS_MASK)))
#define SCE_NP_TROPHY_FLAG_ISSET(n, p) ((p)->flagBits[(n) >> SCE_NP_TROPHY_FLAG_BITS_SHIFT] & (1 << ((n) & SCE_NP_TROPHY_FLAG_BITS_MASK)))
#define SCE_NP_TROPHY_FLAG_ZERO(p)                                                                                                                             \
  do {                                                                                                                                                         \
    SceNpTrophyFlagArray* __fa = (p);                                                                                                                          \
    SceNpTrophyFlagMask   __i;                                                                                                                                 \
    for (__i = 0; __i < (SCE_NP_TROPHY_FLAG_SETSIZE >> SCE_NP_TROPHY_FLAG_BITS_SHIFT); __i++)                                                                  \
      __fa->flagBits[__i] = 0;                                                                                                                                 \
  } while (0)
#define SCE_NP_TROPHY_FLAG_SET_ALL(p)                                                                                                                          \
  do {                                                                                                                                                         \
    SceNpTrophyFlagArray* __fa = (p);                                                                                                                          \
    SceNpTrophyFlagMask   __i;                                                                                                                                 \
    for (__i = 0; __i < (SCE_NP_TROPHY_FLAG_SETSIZE >> SCE_NP_TROPHY_FLAG_BITS_SHIFT); __i++)                                                                  \
      __fa->flagBits[__i] = SCE_NP_TROPHY_FLAG_BITS_ALL;                                                                                                       \
  } while (0)

typedef struct SceNpTrophyGameDetails {
  size_t   size;
  uint32_t numGroups;
  uint32_t numTrophies;
  uint32_t numPlatinum;
  uint32_t numGold;
  uint32_t numSilver;
  uint32_t numBronze;
  char     title[SCE_NP_TROPHY_GAME_TITLE_MAX_SIZE];
  char     description[SCE_NP_TROPHY_GAME_DESCR_MAX_SIZE];
} SceNpTrophyGameDetails;

typedef struct SceNpTrophyGameData {
  size_t   size;
  uint32_t unlockedTrophies;
  uint32_t unlockedPlatinum;
  uint32_t unlockedGold;
  uint32_t unlockedSilver;
  uint32_t unlockedBronze;
  uint32_t progressPercentage;
} SceNpTrophyGameData;

typedef struct SceNpTrophyGroupDetails {
  size_t             size;
  SceNpTrophyGroupId groupId;
  uint32_t           numTrophies;
  uint32_t           numPlatinum;
  uint32_t           numGold;
  uint32_t           numSilver;
  uint32_t           numBronze;
  char               title[SCE_NP_TROPHY_GROUP_TITLE_MAX_SIZE];
  char               description[SCE_NP_TROPHY_GROUP_DESCR_MAX_SIZE];
} SceNpTrophyGroupDetails;

typedef struct SceNpTrophyGroupData {
  size_t             size;
  SceNpTrophyGroupId groupId;
  uint32_t           unlockedTrophies;
  uint32_t           unlockedPlatinum;
  uint32_t           unlockedGold;
  uint32_t           unlockedSilver;
  uint32_t           unlockedBronze;
  uint32_t           progressPercentage;
  uint8_t            reserved[4];
} SceNpTrophyGroupData;

typedef struct SceNpTrophyDetails {
  size_t             size;
  SceNpTrophyId      trophyId;
  SceNpTrophyGrade   trophyGrade;
  SceNpTrophyGroupId groupId;
  bool               hidden;
  uint8_t            reserved[3];
  char               name[SCE_NP_TROPHY_NAME_MAX_SIZE];
  char               description[SCE_NP_TROPHY_DESCR_MAX_SIZE];
} SceNpTrophyDetails;

typedef struct SceNpTrophyData {
  size_t        size;
  SceNpTrophyId trophyId;
  bool          unlocked;
  uint8_t       reserved[3];
  uint64_t      timestamp;
} SceNpTrophyData;

typedef struct SceNpTrophyScreenshotTarget {
  SceNpTrophyContext context;
  SceNpTrophyId      trophyId;
} SceNpTrophyScreenshotTarget;