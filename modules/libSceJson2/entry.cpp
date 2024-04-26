#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

#include <new>

LOG_DEFINE_MODULE(libSceJson2);

namespace sce {
namespace Json {

/* Memory allocator things*/
MemAllocator::MemAllocator() {}

MemAllocator::~MemAllocator() {}

void MemAllocator::notifyError(int32_t error, size_t size, void* userData) {}

/* Initializer parameter things */
InitParameter2::InitParameter2() {}

void InitParameter2::setAllocator(MemAllocator* alloc, void* userData) {}

void InitParameter2::setFileBufferSize(size_t size) {}

void InitParameter2::setSpecialFloatFormatType(SpecialFloatFormat fmt) {}

MemAllocator* InitParameter2::getAllocator() const {
  return nullptr;
}

void* InitParameter2::getUserData() const {
  return nullptr;
}

size_t InitParameter2::getFileBufferSize() const {
  return 0;
}

SpecialFloatFormat InitParameter2::getSpecialFloatFormatType() const {
  return SpecialFloatFormat::FloatNull;
}

/* Initializer things */
Initializer::Initializer() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

Initializer::~Initializer() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

int32_t Initializer::initialize(const InitParameter*) {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 0;
}

int32_t Initializer::initialize(const InitParameter2* param) {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 0;
}

int32_t Initializer::terminate() {
  return 0;
}

int32_t Initializer::setGlobalTypeMismatchHandler(void* /* todo func typedef */ func, void* context) {
  return 0;
}

int32_t Initializer::setGlobalNullAccessCallback(void* /* todo func typedef */ func, void* context) {
  return 0;
}

int32_t Initializer::setGlobalSpecialFloatHandler(void* /* todo func typedef */ func, void* context) {
  return 0;
}

int32_t Initializer::setGlobalElementAccessFailureHandler(void* /* todo func typedef */ func, void* context) {
  return 0;
}

int32_t Initializer::setAllocatorInfoCallback(AllocInfoCallback func, void* context) {
  return 0;
}
} // namespace Json
} // namespace sce

extern "C" {

EXPORT const char* MODULE_NAME = "libSceJson";

/**
 * @brief sce::Json::MemAllocator::MemAllocator()
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json12MemAllocatorC2Ev)(sce::Json::MemAllocator* _this) {
  ::memset((void*)_this, 0, sizeof(sce::Json::MemAllocator));
}

/**
 * @brief sce::Json::Value::Value()
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json5ValueC1Ev)(sce::Json::Value* _this) {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  ::memset((void*)_this, 0, sizeof(sce::Json::Value));
}

/**
 * @brief sce::Json::Value::set(bool)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json5Value3setEb)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::Value::set(long)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json5Value3setEl)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::Value::set(unsigned long)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json5Value3setEm)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::Value::set(double)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json5Value3setEd)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::Value::~Value()
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json5ValueD1Ev)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

/**
 * @brief sce::Json::Value::set(sce::Json::String const&)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json5Value3setERKNS0_6StringE)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::String::String(char const*)
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json6StringC1EPKc)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

/**
 * @brief sce::Json::Value::set(sce::Json::ValueType)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json5Value3setENS0_9ValueTypeE)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::String::~String()
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json6StringD1Ev)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

/**
 * @brief sce::Json::InitParameter2::InitParameter2()
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json14InitParameter2C1Ev)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::InitParameter2::setFileBufferSize(unsigned long)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json14InitParameter217setFileBufferSizeEm)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::InitParameter2::setAllocator(sce::Json::MemAllocator*, void*)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json14InitParameter212setAllocatorEPNS0_12MemAllocatorEPv)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

/**
 * @brief sce::Json::Initializer::Initializer()
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json11InitializerC1Ev)(sce::Json::Initializer* _this) {
  ::memset((void*)_this, 0, sizeof(sce::Json::Initializer));
  new (_this) sce::Json::Initializer();
}

/**
 * @brief sce::Json::Initializer::~Initializer()
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json11InitializerD1Ev)(sce::Json::Initializer* _this) {
  _this->~Initializer();
}

/**
 * @brief sce::Json::MemAllocator::notifyError(int, unsigned long, void*)
 *
 */
EXPORT SYSV_ABI void __NID(_ZN3sce4Json12MemAllocator11notifyErrorEimPv)(sce::Json::MemAllocator* _this, int32_t error, size_t size, void* userData) {
  return _this->notifyError(error, size, userData);
}

EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json11Initializer10initializeEPKNS0_13InitParameterE)(sce::Json::Initializer*         _this,
                                                                                            sce::Json::InitParameter const* param) {
  return _this->initialize(param);
}

/**
 * @brief sce::Json::Initializer::initialize(sce::Json::InitParameter2 const*)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json11Initializer10initializeEPKNS0_14InitParameter2E)(sce::Json::Initializer*          _this,
                                                                                             sce::Json::InitParameter2 const* param) {
  return _this->initialize(param);
}

/**
 * @brief sce::Json::Initializer::setGlobalNullAccessCallback(sce::Json::Value const& (*)(sce::Json::ValueType, sce::Json::Value const*, void*), void*)
 *
 */
EXPORT SYSV_ABI int32_t __NID(_ZN3sce4Json11Initializer27setGlobalNullAccessCallbackEPFRKNS0_5ValueENS0_9ValueTypeEPS3_PvES7_)() {
  LOG_USE_MODULE(libSceJson2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
