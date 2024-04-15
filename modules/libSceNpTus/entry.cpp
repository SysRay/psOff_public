#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpTus);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpTus";

EXPORT SYSV_ABI int32_t sceNpTusAbortRequest() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusAddAndGetVariableVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusChangeModeForOtherSaveDataOwners() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusCreateNpTitleCtx() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusCreateNpTitleCtxA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusCreateRequest() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusCreateTitleCtx() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotData() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotDataA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotDataAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotDataAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotDataVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotDataVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotVariableVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteMultiSlotVariableVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteNpTitleCtx() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusDeleteRequest() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetData() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetDataVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsDataStatus() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsDataStatusA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsDataStatusAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsDataStatusAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsDataStatusForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsDataStatusForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsVariableForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetFriendsVariableForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatus() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotDataStatusVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t ceNpTusGetMultiSlotVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiSlotVariableVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatus() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserDataStatusVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusGetMultiUserVariableVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusPollAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetData() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetDataVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariableVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetMultiSlotVariableVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetThreadParam() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusSetTimeout() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariable() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableA() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableAAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableAVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableAVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableForCrossSave() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableForCrossSaveAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableForCrossSaveVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableForCrossSaveVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableVUser() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusTryAndSetVariableVUserAsync() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpTusWaitAsync() {
  return Ok;
}
}
