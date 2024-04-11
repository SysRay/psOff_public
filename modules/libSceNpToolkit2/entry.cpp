#include "common.h"
#include "errorcodes.h"
#include "logging.h"

LOG_DEFINE_MODULE(libSceNpToolkit2);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpToolkit2";

/**
 * @brief sce::Toolkit::NP::V2::Core::ResponseBase::getReturnCode()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core12ResponseBase13getReturnCodeEv)(void* param) {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::init(sce::Toolkit::NP::V2::Core::Request::InitParams const&)
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core4initERKNS3_7Request10InitParamsE()) {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::Request::InitParams::InitParams()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core7Request10InitParamsC1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::Request::InitParams::~InitParams()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core7Request10InitParamsD1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::Request::ServerPushNotifications::ServerPushNotifications()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core7Request23ServerPushNotificationsC1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::Request::ServerPushNotifications::~ServerPushNotifications()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core7Request23ServerPushNotificationsD1Ev)(int32_t handle) {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::Response<sce::Toolkit::NP::V2::Core::Empty>::Response()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core8ResponseINS3_5EmptyEEC1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Core::Response<sce::Toolkit::NP::V2::Core::Empty>::~Response()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V24Core8ResponseINS3_5EmptyEED1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::registerTrophyPack(sce::Toolkit::NP::V2::Trophy::Request::RegisterTrophyPack const&,
 * sce::Toolkit::NP::V2::Core::Response<sce::Toolkit::NP::V2::Core::Empty>*)
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy18registerTrophyPackERKNS3_7Request18RegisterTrophyPackEPNS2_4Core8ResponseINS8_5EmptyEEE)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::unlock(sce::Toolkit::NP::V2::Trophy::Request::Unlock const&,
 * sce::Toolkit::NP::V2::Core::Response<sce::Toolkit::NP::V2::Core::Empty>*)
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy6unlockERKNS3_7Request6UnlockEPNS2_4Core8ResponseINS8_5EmptyEEE)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::Request::RegisterTrophyPack::RegisterTrophyPack()
 *
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy7Request18RegisterTrophyPackC1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::Request::RegisterTrophyPack::~RegisterTrophyPack()
 *
 */

EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy7Request18RegisterTrophyPackD1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::Request::Unlock::Unlock()
 *
 */

EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy7Request6UnlockC1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::Request::Unlock::~Unlock()
 *
 */

EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy7Request6UnlockD1Ev)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(58C6C6855C0C06B4)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(638F8DA8E21E74E9)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(50E54F00733C647C)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(77CD34F46FBA6D1C)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(605F1F6FAB19BE1E)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(4E91B85064870436)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(B71EFC50D1BA6283)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(816A0A203650C902)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(0800693AF1D10D52)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(F322056CA9E17F6F)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(61BAC13A7A85D4E8)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(CD2E535CE57B90)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(CD2E535CE57B902C)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(03B5110FB508E5F1)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(80070E81DD56719E)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(F53AE1B86CDB7AB4)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(960002C58042805C)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(2DF8DA7BE307EB18)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(4EC11A5B9100EA24)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(741F676AE016B608)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(2AA954247FAE01EA)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(6FD21800DDAB865A)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t __NID_HEX(65611C8392EA75F9)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
