#include "common.h"
#include "core/fileManager/fileManager.h"
#include "core/imports/exports/procParam.h"
#include "core/imports/exports/runtimeExport.h"
#include "core/imports/imports_runtime.h"
#include "core/kernel/errors.h"
#include "core/memory/memory.h"
#include "core/timer/timer.h"
#include "logging.h"
#include "types.h"

#include <algorithm>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <windows.h>
#undef min
LOG_DEFINE_MODULE(libkernel);

namespace {

static get_thread_atexit_count_func_t g_get_thread_atexit_count_func = nullptr;
static thread_atexit_report_func_t    g_thread_atexit_report_func    = nullptr;
} // namespace

// ### OBJECTS
extern "C" {
EXPORT uint64_t __NID(__stack_chk_guard) = 0xDeadBeef5533CCAA;

EXPORT const char* __NID(__progname) = "emulator";
}
// - OBJECTS

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

SYSV_ABI int* __NID(__error()) {
  return getError_pthread(); // todo: or extra for kernel?
}

EXPORT SYSV_ABI int __NID(getargc)(void) {
  return accessRuntimeExport()->getEntryParams()->argc;
}

EXPORT SYSV_ABI char const* const* __NID(getargv)(void) {
  return accessRuntimeExport()->getEntryParams()->argv;
}

EXPORT SYSV_ABI int __NID(getpagesize)(void) {
  return memory::getpagesize();
}

EXPORT SYSV_ABI void* __NID(__tls_get_addr)(TlsInfo* info) {
  return accessRuntimeExport()->getTLSAddr(info->index, info->offset);
}

EXPORT SYSV_ABI void __NID(__stack_chk_fail)() {
  LOG_USE_MODULE(libkernel);
  LOG_CRIT(L"Stack check fail");
}

EXPORT SYSV_ABI void __NID(_exit)(int code) {
  LOG_USE_MODULE(libkernel);
  LOG_INFO(L"exit code:%d", code);

  ::exit(code);
}

EXPORT SYSV_ABI uint8_t __NID(_is_signal_return)(int64_t* param) {
  uint8_t retVal = 0;

  if (((*param != 0x48006a40247c8d48) || (param[1] != 0x50f000001a1c0c7)) || (retVal = 1, (param[2] & 0xffffffU) != 0xfdebf4)) {
    retVal = ((*(uint64_t*)((int64_t)param + -5) & 0xffffffffff) == 0x50fca8949) * 2;
  }
  return retVal;
}

EXPORT SYSV_ABI int __NID(sigprocmask)(int /*how*/, const void* /*set*/, void* /*oset*/) {
  LOG_USE_MODULE(libkernel);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return 0;
}

EXPORT SYSV_ABI int __NID(_sigprocmask)(int how, const void* set, void* oset) {
  return __NID(sigprocmask)(how, set, oset);
}

EXPORT SYSV_ABI int __NID(sigfillset)(sigset_t* set) {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return 0;
}

EXPORT SYSV_ABI void* _sceModuleParam() {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__); // todo what module?
  auto const procParamVaddr = accessRuntimeExport()->mainModuleInfo().procParamAddr;
  return reinterpret_cast<void*>(procParamVaddr);
}

EXPORT SYSV_ABI int sceKernelInternalMemoryGetModuleSegmentInfo(ModulInfo* info) {
  if (info == nullptr) return getErr(ErrCode::_EFAULT);

  *info = accessRuntimeExport()->mainModuleInfo();
  return Ok;
}

EXPORT SYSV_ABI int sceKernelConvertUtcToLocaltime(time_t time, time_t* local_time, struct SceTimesec* st, unsigned long* dstsec) {
  const auto* tz      = std::chrono::current_zone();
  auto        sys_inf = tz->get_info(std::chrono::system_clock::now());

  *local_time = sys_inf.offset.count() + sys_inf.save.count() * 60 + time;

  if (st != nullptr) {
    st->t       = time;
    st->westsec = sys_inf.offset.count() * 60;
    st->dstsec  = sys_inf.save.count() * 60;
  }

  if (dstsec != nullptr) {
    *dstsec = sys_inf.save.count() * 60;
  }

  return Ok;
}

EXPORT SYSV_ABI int sceKernelConvertLocaltimeToUtc() {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI unsigned int sceKernelSleep(unsigned int seconds) {
  boost::this_thread::sleep_for(boost::chrono::seconds(seconds));
  return Ok;
}

EXPORT SYSV_ABI int sceKernelUsleep(SceKernelUseconds microseconds) {
  boost::this_thread::sleep_for(boost::chrono::microseconds(microseconds));
  return Ok;
}

EXPORT SYSV_ABI int sceKernelNanosleep(const SceKernelTimespec* rqtp, SceKernelTimespec* rmtp) {
  auto startTime = boost::chrono::high_resolution_clock::now();

  boost::this_thread::sleep_for(boost::chrono::seconds(rqtp->tv_sec) + boost::chrono::nanoseconds(rqtp->tv_nsec));

  if (rmtp != nullptr) {
    auto const endTime = boost::chrono::high_resolution_clock::now();
    auto const diff    = boost::chrono::duration_cast<boost::chrono::nanoseconds>(endTime - startTime).count();
    ns2timespec(rmtp, diff);
  }
  return Ok;
}

EXPORT SYSV_ABI int sceKernelClockGetres(SceKernelClockid clockId, SceKernelTimespec* tp) {
  return accessTimer().getTimeRes(clockId, tp);
}

EXPORT SYSV_ABI int sceKernelClockGettime(SceKernelClockid clockId, SceKernelTimespec* tp) {
  return accessTimer().getTime(clockId, tp);
}

EXPORT SYSV_ABI int sceKernelGettimeofday(SceKernelTimeval* tp) {
  return accessTimer().getTimeofDay(tp);
}

EXPORT SYSV_ABI int sceKernelGettimezone(SceKernelTimezone* tz) {
  return accessTimer().getTimeZone(tz);
}

EXPORT SYSV_ABI int __NID(clock_getres)(SceKernelClockid clockId, SceKernelTimespec* tp) {
  return accessTimer().getTimeRes(clockId, tp);
}

EXPORT SYSV_ABI int __NID(clock_gettime)(SceKernelClockid clockId, SceKernelTimespec* tp) {
  return accessTimer().getTime(clockId, tp);
}

EXPORT SYSV_ABI int __NID(clock_settime)(SceKernelTimeval* tp) {
  return accessTimer().getTimeofDay(tp);
}

EXPORT SYSV_ABI uint64_t sceKernelGetTscFrequency(void) {
  return accessTimer().getFrequency();
}

EXPORT SYSV_ABI uint64_t sceKernelReadTsc(void) {
  return accessTimer().queryPerformance();
}

EXPORT SYSV_ABI uint64_t sceKernelGetProcessTime(void) {
  return (uint64_t)(1e3 * accessTimer().getTimeMs());
}

EXPORT SYSV_ABI uint64_t sceKernelGetProcessTimeCounter(void) {
  return accessTimer().getTicks();
}

EXPORT SYSV_ABI uint64_t sceKernelGetProcessTimeCounterFrequency(void) {
  return accessTimer().getFrequency();
}

EXPORT SYSV_ABI int sceKernelGetCurrentCpu(void) {
  return GetCurrentProcessorNumber();
}

EXPORT SYSV_ABI SceKernelModule sceKernelLoadStartModule(const char* moduleFileName, size_t args, const void* argp, uint32_t flags,
                                                         const SceKernelLoadModuleOpt* pOpt, int* pRes) {
  auto mapped = accessFileManager().getMappedPath(moduleFileName);
  if (!mapped) {
    return getErr(ErrCode::_EACCES);
  }

  auto id = accessRuntimeExport()->loadStartModule(*mapped, args, argp, pRes);
  return id != 0 ? id : getErr(ErrCode::_EACCES);
}

EXPORT SYSV_ABI int sceKernelStopUnloadModule(SceKernelModule handle, size_t args, const void* argp, uint32_t flags, const SceKernelUnloadModuleOpt* pOpt,
                                              int* pRes) {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI void sceKernelSetGPO(uint32_t uiBits) {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI uint64_t sceKernelGetGPI() {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelSetFsstParam(int prio, SceKernelCpumask mask) {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelSetProcessProperty() {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelGetCpumode() {
  return (int)SceKernelCPUMode::CPUMODE_6CPU;
}

EXPORT SYSV_ABI int sceKernelIsNeoMode() {
  // return (Config::IsNeo() ? 1 : 0); // todo
  return 0;
}

EXPORT SYSV_ABI int sceKernelMprotect(uint64_t addr, size_t len, int prot) {
  return memory::protect(addr, len, prot, nullptr) ? Ok : getErr(ErrCode::_EACCES);
}

EXPORT SYSV_ABI int sceKernelMsync(void* addr, size_t len, int flags) {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceKernelUuidCreate(uint8_t* uuid) {
  if (uuid == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  for (auto const byte: boost::uuids::random_generator()()) {
    *uuid++ = byte;
  }
  return Ok;
}

EXPORT SYSV_ABI void sceKernelDebugRaiseExceptionOnReleaseMode(int /*c1*/, int /*c2*/) {
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S", __FUNCTION__);
}

EXPORT SYSV_ABI void _sceKernelSetThreadAtexitReport(thread_atexit_report_func_t func) {
  g_thread_atexit_report_func = func;
}

EXPORT SYSV_ABI int _sceKernelRtldThreadAtexitIncrement(uint64_t* /*c*/) {
  return Ok;
}

EXPORT SYSV_ABI int _sceKernelRtldThreadAtexitDecrement(uint64_t* /*c*/) {
  return Ok;
}

EXPORT SYSV_ABI SceMallocReplace* sceKernelGetSanitizerMallocReplaceExternal() {
  return nullptr;
}

EXPORT SYSV_ABI SceLibcNewReplace* sceKernelGetSanitizerNewReplaceExternal() {
  return nullptr;
}

EXPORT SYSV_ABI int sceKernelIsAddressSanitizerEnabled() {
  return 0;
}

EXPORT SYSV_ABI void _sceKernelSetThreadAtexitCount(get_thread_atexit_count_func_t func) {
  g_get_thread_atexit_count_func = func;
}

EXPORT SYSV_ABI void _sceKernelRtldSetApplicationHeapAPI(void* api[]) {
  auto& heap_malloc         = api[0];
  auto& heap_free           = api[1];
  auto& heap_posix_memalign = api[6];
}

EXPORT SYSV_ABI void sceKernelDebugRaiseException(int reason, int id) {
  LOG_USE_MODULE(libkernel);
  LOG_CRIT(L"Exception: reason:0x%lx id:0x%lx", reason, id);
}

EXPORT SYSV_ABI int __NID(__elf_phdr_match_addr)(SceKernelModuleInfoEx* m, uint64_t dtor_vaddr) {
  LOG_USE_MODULE(libkernel);
  if (m->segment_count == 0) return 0;

  int result = 0;
  if (m->segment_count != 0) {
    for (int n = 0; n < m->segment_count; ++n) {
      auto& seg = m->segments[n];
      if ((seg.prot & SceProtExecute) != 0) {
        if (seg.address <= dtor_vaddr && (seg.address + seg.size) > dtor_vaddr) {
          result = n != m->segment_count ? 1 : 0;
          break;
        }
      }
    }
  }

  LOG_INFO(L"elf_phdr_match_addr dtor_vaddr:0x%08llx match:%d", dtor_vaddr, result);

  return result;
}

EXPORT SYSV_ABI int sceKernelGetModuleInfo(SceKernelModule handle, SceKernelModuleInfo* r) {
  if (r == nullptr) return getErr(ErrCode::_EFAULT);
  if (r->size < sizeof(*r)) return getErr(ErrCode::_EINVAL);
  LOG_USE_MODULE(libkernel);
  LOG_ERR(L"todo %S(%d)", __FUNCTION__, handle);
  // auto info = accessRuntimeExport()->getModuleInfo(handle); // todo
  return Ok;
}

EXPORT SYSV_ABI int sceKernelGetModuleInfoFromAddr(uint64_t addr, int n, SceKernelModuleInfoEx* r) {
  if (r == nullptr) return getErr(ErrCode::_EFAULT);
  if (r->size < sizeof(*r)) return getErr(ErrCode::_EINVAL);
  auto info = accessRuntimeExport()->getModuleInfoEx(addr);
  if (info == nullptr) {
    r->id = 0;
    return getErr(ErrCode::_EINVAL);
  }

  *r = *info;
  return Ok;
}

EXPORT SYSV_ABI int sceKernelGetModuleInfoForUnwind(uint64_t addr, int n, SceModuleUndwindInfo* r) {
  if (r == nullptr) return getErr(ErrCode::_EFAULT);
  if (r->size < sizeof(*r)) return getErr(ErrCode::_EINVAL);

  auto info = accessRuntimeExport()->getModuleInfoEx(addr);
  if (info == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  strcpy_s(r->name, info->name);

  r->eh_frame_hdr_addr = info->eh_frame_hdr_addr;
  r->eh_frame_addr     = info->eh_frame_addr;
  r->eh_frame_size     = info->eh_frame_size;
  r->seg0Addr          = info->segments[0].address;
  r->seg0Size          = info->segments[0].size;
  return Ok;
}

EXPORT SYSV_ABI void* sceKernelGetProcParam() {
  return (void*)accessRuntimeExport()->mainModuleInfo().procParamAddr;
}

EXPORT SYSV_ABI int sceKernelGetModuleList(int* modules, size_t size, size_t* sizeOut) {
  auto const modules_ = accessRuntimeExport()->getModules();

  for (size_t n = 0; n < std::min(size, modules_.size()); ++n) {
    modules[n] = modules_[n];
  }
  *sizeOut = modules_.size();
  if (modules_.size() > size) return getErr(ErrCode::_ENOMEM);

  return Ok;
}

EXPORT SYSV_ABI int sceKernelGetCompiledSdkVersion(int* ver) {
  *ver = 0x7FFFF;
  return Ok;
}

EXPORT SYSV_ABI int sceKernelDlsym(int moduleId, const char* symbol, uint64_t* pAddr) {
  *pAddr = (uint64_t)accessRuntimeExport()->getSymbol(moduleId, symbol, false);
  LOG_USE_MODULE(libkernel);
  LOG_DEBUG(L"dlsym[%d] 0x%08llx %S", moduleId, *pAddr, symbol);
  if (*pAddr == 0) return getErr(ErrCode::_EFAULT);
  return Ok;
}

EXPORT SYSV_ABI int __NID(getrusage)(rusageWho who, rusage_t* usage) {
  *usage = rusage_t();
  return Ok;
}
}
