#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpScore);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpScore";

EXPORT SYSV_ABI int sceNpScoreCreateNpTitleCtx(SceNpServiceLabel serviceLabel, SceUserServiceUserId selfId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 1;
}

EXPORT SYSV_ABI int sceNpScoreCreateNpTitleCtxA(SceNpServiceLabel serviceLabel, SceUserServiceUserId selfId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 1;
}

EXPORT SYSV_ABI int sceNpScoreDeleteNpTitleCtx(int32_t titleCtxId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreCreateRequest(int32_t titleCtxId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreDeleteRequest(int32_t reqId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreAbortRequest(int32_t reqId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreSetTimeout(int32_t id, int32_t resolveRetry, int32_t resolveTimeout, int32_t connTimeout, int32_t sendTimeout,
                                         int32_t recvTimeout) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreSetThreadParam(int32_t threadPriority, SceKernelCpumask cpuAffinityMask) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreSetPlayerCharacterId(int32_t id, SceNpScorePcId pcId) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetBoardInfo(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreBoardInfo* boardInfo, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreRecordScore(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreValue score, const SceNpScoreComment* scoreComment,
                                          const SceNpScoreGameInfo* gameInfo, SceNpScoreRankNumber* tmpRank, const SceRtcTick* compareDate, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreRecordGameData(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreValue score, size_t totalSize, size_t sendSize,
                                             const void* data, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetGameDataByAccountId(int32_t reqId, SceNpScoreBoardId boardId, SceNpAccountId accountId, size_t* totalSize, size_t recvSize,
                                                     void* data, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetGameDataByAccountIdAsync(int32_t reqId, SceNpScoreBoardId boardId, SceNpAccountId accountId, size_t* totalSize,
                                                          size_t recvSize, void* data, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByNpId() {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByNpIdAsync() {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountId(int32_t reqId, SceNpScoreBoardId boardId, const SceNpAccountId accountIdArray[], size_t accountIdArraySize,
                                                    SceNpScorePlayerRankDataA* rankArray, size_t rankArraySize, SceNpScoreComment* commentArray,
                                                    size_t commentArraySize, SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                    SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdForCrossSave(int32_t reqId, SceNpScoreBoardId boardId, const SceNpAccountId accountIdArray[],
                                                                size_t accountIdArraySize, SceNpScorePlayerRankDataForCrossSave* rankArray,
                                                                size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize,
                                                                SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate,
                                                                SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByRangeA(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreRankNumber startSerialRank, SceNpScoreRankDataA* rankArray,
                                                 size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                 size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord,
                                                 void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByRangeForCrossSave(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreRankNumber startSerialRank,
                                                            SceNpScoreRankDataForCrossSave* rankArray, size_t rankArraySize, SceNpScoreComment* commentArray,
                                                            size_t commentArraySize, SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                            SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdPcId(int32_t reqId, SceNpScoreBoardId boardId, const SceNpScoreAccountIdPcId* idArray, size_t idArraySize,
                                                        SceNpScorePlayerRankDataA* rankArray, size_t rankArraySize, SceNpScoreComment* commentArray,
                                                        size_t commentArraySize, SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                        SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdPcIdForCrossSave(int32_t reqId, SceNpScoreBoardId boardId, const SceNpScoreAccountIdPcId* idArray,
                                                                    size_t idArraySize, SceNpScorePlayerRankDataForCrossSave* rankArray, size_t rankArraySize,
                                                                    SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                                    size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate,
                                                                    SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreCensorComment(int32_t reqId, const char* comment, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreSanitizeComment(int32_t reqId, const char* comment, char* sanitizedComment, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreWaitAsync(int32_t id, int32_t* result) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScorePollAsync(int32_t reqId, int32_t* result) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetBoardInfoAsync(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreBoardInfo* boardInfo, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreRecordScoreAsync(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreValue score, const SceNpScoreComment* scoreComment,
                                               const SceNpScoreGameInfo* gameInfo, SceNpScoreRankNumber* tmpRank, const SceRtcTick* compareDate, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreRecordGameDataAsync(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreValue score, size_t totalSize, size_t sendSize,
                                                  const void* data, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdAsync(int32_t reqId, SceNpScoreBoardId boardId, const SceNpAccountId accountIdArray[],
                                                         size_t accountIdArraySize, SceNpScorePlayerRankDataA* rankArray, size_t rankArraySize,
                                                         SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                         size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord,
                                                         void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdForCrossSaveAsync(int32_t reqId, SceNpScoreBoardId boardId, const SceNpAccountId accountIdArray[],
                                                                     size_t accountIdArraySize, SceNpScorePlayerRankDataForCrossSave* rankArray,
                                                                     size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize,
                                                                     SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                                     SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByRangeAAsync(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreRankNumber startSerialRank,
                                                      SceNpScoreRankDataA* rankArray, size_t rankArraySize, SceNpScoreComment* commentArray,
                                                      size_t commentArraySize, SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                      SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByRangeForCrossSaveAsync(int32_t reqId, SceNpScoreBoardId boardId, SceNpScoreRankNumber startSerialRank,
                                                                 SceNpScoreRankDataForCrossSave* rankArray, size_t rankArraySize,
                                                                 SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                                 size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate,
                                                                 SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdPcIdAsync(int32_t reqId, SceNpScoreBoardId boardId, const SceNpScoreAccountIdPcId* idArray,
                                                             size_t idArraySize, SceNpScorePlayerRankDataA* rankArray, size_t rankArraySize,
                                                             SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                             size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord,
                                                             void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByAccountIdPcIdForCrossSaveAsync(int32_t reqId, SceNpScoreBoardId boardId, const SceNpScoreAccountIdPcId* idArray,
                                                                         size_t idArraySize, SceNpScorePlayerRankDataForCrossSave* rankArray,
                                                                         size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize,
                                                                         SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                                         SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetFriendsRanking(/*int32_t reqId, SceNpScoreBoardId boardId, int32_t includeSelf, SceNpScoreRankDataA* rankArray,
                                                 size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                 size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord,
                                                 SceNpScoreGetFriendRankingOptParam* option*/) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetFriendsRankingAsync() {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetFriendsRankingA(int32_t reqId, SceNpScoreBoardId boardId, int32_t includeSelf, SceNpScoreRankDataA* rankArray,
                                                 size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                 size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord,
                                                 SceNpScoreGetFriendRankingOptParam* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetFriendsRankingAAsync(int32_t reqId, SceNpScoreBoardId boardId, int32_t includeSelf, SceNpScoreRankDataA* rankArray,
                                                      size_t rankArraySize, SceNpScoreComment* commentArray, size_t commentArraySize,
                                                      SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate,
                                                      SceNpScoreRankNumber* totalRecord, SceNpScoreGetFriendRankingOptParam* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetFriendsRankingForCrossSave(int32_t reqId, SceNpScoreBoardId boardId, int32_t includeSelf,
                                                            SceNpScoreRankDataForCrossSave* rankArray, size_t rankArraySize, SceNpScoreComment* commentArray,
                                                            size_t commentArraySize, SceNpScoreGameInfo* infoArray, size_t infoArraySize, size_t arrayNum,
                                                            SceRtcTick* lastSortDate, SceNpScoreRankNumber* totalRecord,
                                                            SceNpScoreGetFriendRankingOptParam* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetFriendsRankingForCrossSaveAsync(int32_t reqId, SceNpScoreBoardId boardId, int32_t includeSelf,
                                                                 SceNpScoreRankDataForCrossSave* rankArray, size_t rankArraySize,
                                                                 SceNpScoreComment* commentArray, size_t commentArraySize, SceNpScoreGameInfo* infoArray,
                                                                 size_t infoArraySize, size_t arrayNum, SceRtcTick* lastSortDate,
                                                                 SceNpScoreRankNumber* totalRecord, SceNpScoreGetFriendRankingOptParam* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreCensorCommentAsync(int32_t reqId, const char* comment, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreSanitizeCommentAsync(int32_t reqId, const char* comment, char* sanitizedComment, void* option) {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByRange() {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceNpScoreGetRankingByRangeAsync() {
  LOG_USE_MODULE(libSceNpScore);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
