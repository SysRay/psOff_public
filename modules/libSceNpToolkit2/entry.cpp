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
 * @brief sce::Toolkit::NP::V2::Trophy::registerTrophyPack(sce::Toolkit::NP::V2::Trophy::Request::RegisterTrophyPack const&, sce::Toolkit::NP::V2::Core::Response<sce::Toolkit::NP::V2::Core::Empty>*)
 * 
 */
EXPORT SYSV_ABI int __NID(_ZN3sce7Toolkit2NP2V26Trophy18registerTrophyPackERKNS3_7Request18RegisterTrophyPackEPNS2_4Core8ResponseINS8_5EmptyEEE)() {
  LOG_USE_MODULE(libSceNpToolkit2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Toolkit::NP::V2::Trophy::unlock(sce::Toolkit::NP::V2::Trophy::Request::Unlock const&, sce::Toolkit::NP::V2::Core::Response<sce::Toolkit::NP::V2::Core::Empty>*)
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
}
