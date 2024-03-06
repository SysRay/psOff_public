#include "common.h"
#include "core/kernel/pthread.h"
#include "logging.h"
#include "types.h"

#include <boost/thread.hpp>
#include <windows.h>

LOG_DEFINE_MODULE(libkernel_unity);

namespace {

VOID NTAPI SignalHandler(_In_ ULONG_PTR _signo);

class Signal {
  public:
  auto getHandler(int signo) const { return m_signals[signo]; }

  bool addExceptionHandler(int signo, void* exHandler) {
    LOG_USE_MODULE(libkernel_unity);
    boost::unique_lock lock(m_mutex);

    m_signals[signo] = (unity_EXCEPTION_HANDLER)exHandler;
    LOG_INFO(L"+Exceptionshandler signo:%d handler:0x%08llx", signo, exHandler);
    return true;
  }

  bool removeExceptionHandler(int signo) {
    LOG_USE_MODULE(libkernel_unity);
    boost::unique_lock lock(m_mutex);
    LOG_INFO(L"-Exceptionshandler signo:%d", signo);
    return true;
  }

  bool raise(void* pthread, int signo) {
    LOG_USE_MODULE(libkernel_unity);
    boost::unique_lock lock(m_mutex);

    auto const threadId = pthread::getThreadId((ScePthread_obj)pthread);

    LOG_DEBUG(L"raise signal:%d thread:%d(from:%d)", signo, threadId, pthread::getThreadId());
    pthread::raise((ScePthread_obj)pthread, (void*)SignalHandler, signo);
    return true;
  }

  private:
  boost::mutex m_mutex;

  std::array<unity_EXCEPTION_HANDLER, 33> m_signals;
};

Signal* accessSignals() {
  static Signal obj;
  return &obj;
}

VOID NTAPI SignalHandler(_In_ ULONG_PTR _signo) {
  auto const signo = (int)_signo;

  auto handler = ((Signal*)accessSignals())->getHandler(signo);

  LOG_USE_MODULE(libkernel_unity);

  auto info      = std::make_unique<SigInfo>(); // Dont put it on the stack inside the "exception handler"
  info->si_pid   = (decltype(info->si_pid))GetCurrentProcessId();
  info->si_signo = signo;
  info->si_code  = SiCode::SI_USER;

  LOG_DEBUG(L"Interrupted signal:%d thread:%d", signo, pthread::getThreadId());
  handler(signo, info.get());
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

SYSV_ABI int sceKernelInstallExceptionHandler(int signum, void* exHandler) {
  accessSignals()->addExceptionHandler(signum, exHandler);
  return Ok;
}

SYSV_ABI int sceKernelRemoveExceptionHandler(int signum) {
  accessSignals()->removeExceptionHandler(signum);
  return Ok;
}

SYSV_ABI int sceKernelRaiseException(void* pthread, int signum) {
  return accessSignals()->raise(pthread, signum) ? Ok : getErr(ErrCode::_EINVAL);
}
}