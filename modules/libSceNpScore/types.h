#pragma once
#include "codes.h"
#include "common.h"
#include "modules/libSceNpManager/types.h"

typedef uint32_t SceNpScoreBoardId;
typedef int64_t  SceNpScoreValue;
typedef uint32_t SceNpScoreRankNumber;
typedef int32_t  SceNpScorePcId;

struct SceNpScoreGameInfo {
  size_t  infoSize;
  uint8_t data[SCE_NP_SCORE_GAMEINFO_MAXSIZE];
  uint8_t pad2[3];
};

struct SceNpScoreComment {
  char utf8Comment[SCE_NP_SCORE_COMMENT_MAXLEN + 1];
};

struct SceNpScoreRankDataA {
  SceNpOnlineId        onlineId;
  uint8_t              reserved0[16];
  uint8_t              reserved[49];
  uint8_t              pad0[3];
  SceNpScorePcId       pcId;
  SceNpScoreRankNumber serialRank;
  SceNpScoreRankNumber rank;
  SceNpScoreRankNumber highestRank;
  int32_t              hasGameData;
  uint8_t              pad1[4];
  SceNpScoreValue      scoreValue;
  SceRtcTick           recordDate;
  SceNpAccountId       accountId;
  uint8_t              pad2[8];
};

struct SceNpScoreRankDataForCrossSave {
  SceNpId              npId;
  uint8_t              reserved[49];
  uint8_t              pad0[3];
  SceNpScorePcId       pcId;
  SceNpScoreRankNumber serialRank;
  SceNpScoreRankNumber rank;
  SceNpScoreRankNumber highestRank;
  int32_t              hasGameData;
  uint8_t              pad1[4];
  SceNpScoreValue      scoreValue;
  SceRtcTick           recordDate;
  SceNpAccountId       accountId;
  uint8_t              pad2[8];
};

struct SceNpScorePlayerRankDataA {
  int32_t             hasData;
  uint8_t             pad0[4];
  SceNpScoreRankDataA rankData;
};

struct SceNpScorePlayerRankDataForCrossSave {
  int32_t                        hasData;
  uint8_t                        pad0[4];
  SceNpScoreRankDataForCrossSave rankData;
};

struct SceNpScoreBoardInfo {
  uint32_t             rankLimit;
  uint32_t             updateMode;
  uint32_t             sortMode;
  SceNpScoreRankNumber uploadNumLimit;
  size_t               uploadSizeLimit;
};

struct SceNpScoreAccountIdPcId {
  SceNpAccountId accountId;
  SceNpScorePcId pcId;
  uint8_t        pad[4];
};

struct SceNpScoreGetFriendRankingOptParam {
  size_t                size;
  SceNpScoreRankNumber* startSerialRank;
  SceNpScoreRankNumber* hits;
};
