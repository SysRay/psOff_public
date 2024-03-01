#include "common.h"
#include "logging.h"
#include "types.h"

#include <boost/thread.hpp>
#include <memory>
#include <utility/utility.h>
LOG_DEFINE_MODULE(libSceCoredump);

namespace {

typedef SYSV_ABI int (*SceCoredumpHandler)(void* pCommon);

auto getPthread() {
  static boost::thread* g_coreDumHandler = nullptr;
  return g_coreDumHandler;
}

static SceCoredumpHandler g_threadHandler;

void* dummyTask(void* pCommon) {
  util::setThreadName("CoreDumpHandler");
  return (void*)(uint64_t)g_threadHandler(pCommon);
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

EXPORT SYSV_ABI int32_t sceCoredumpRegisterCoredumpHandler(SceCoredumpHandler handler, size_t stackSize, void* pCommon) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);

  g_threadHandler = handler;

  auto threadPriv = getPthread();

  boost::thread::attributes attr;
  attr.set_stack_size(stackSize);

  threadPriv = std::make_unique<boost::thread>(attr, boost::bind(dummyTask, pCommon)).release();

  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpUnregisterCoredumpHandler(void) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);

  auto thread = getPthread();
  thread->interrupt();
  thread->join();
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpSetUserDataType(int type) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S type:%d", __FUNCTION__, type);
  return Ok;
}

EXPORT SYSV_ABI size_t sceCoredumpWriteUserData(const void* data, size_t size) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceCoredumpDebugTextOut(const char* str, int len) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI int32_t sceCoredumpAttachUserFile(uint32_t user_value, const char* path) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpAttachMemoryRegionAsUserFile(uint32_t user_value, const void* mem, size_t size, const char* name) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpAttachMemoryRegion(uint32_t user_value, const void* mem, size_t size) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpAttachUserMemoryFile(uint32_t user_value, const void* mem, size_t size) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpGetStopInfoCpu(SceCoredumpStopInfoCpu* info, size_t size) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpGetStopInfoGpu(SceCoredumpStopInfoGpu* info, size_t size) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCoredumpGetThreadContextInfo(SceCoredumpThreadContextInfo* threadContextInfo, size_t threadContextInfoSize) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceCoredumpDebugTriggerCoredump(void) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI void sceCoredumpDebugForceCoredumpOnAppClose(void) {
  LOG_USE_MODULE(libSceCoredump);
  LOG_ERR(L"todo %S", __FUNCTION__);
}
}