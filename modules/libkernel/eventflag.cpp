#include "core/kernel/eventflag.h"

#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(eventFlag);
using namespace Kernel::EventFlag;

extern "C" {

EXPORT SYSV_ABI int sceKernelCreateEventFlag(IKernelEventFlag_t* ef, const char* name, uint32_t attr, uint64_t init_pattern, const void* param) {
  LOG_USE_MODULE(eventFlag);

  if (ef == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  bool single = true;
  bool fifo   = true;

  switch (attr) {
    case 0x10:
    case 0x11:
      single = true;
      fifo   = true;
      break;
    case 0x20:
    case 0x21:
      single = false;
      fifo   = true;
      break;
    case 0x22:
      single = false;
      fifo   = false;
      break;
    default: LOG_CRIT(L"unknown attr: %u", attr);
  }

  std::string _name;
  if (name != nullptr) _name = std::string(name);
  *ef = createEventFlag(_name, single, fifo, init_pattern);

  return Ok;
}

EXPORT SYSV_ABI int sceKernelDeleteEventFlag(IKernelEventFlag_t ef) {
  return deleteEventFlag(ef);
}

EXPORT SYSV_ABI int sceKernelWaitEventFlag(IKernelEventFlag_t ef, uint64_t bitPattern, uint32_t waitMode, uint64_t* pResultPat, SceKernelUseconds* pTimeout) {
  LOG_USE_MODULE(eventFlag);

  if (ef == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  if (bitPattern == 0) {
    return getErr(ErrCode::_EINVAL);
  }

  WaitMode  wait  = WaitMode::And;
  ClearMode clear = ClearMode::None;

  switch (waitMode & 0xfu) {
    case 0x01: wait = WaitMode::And; break;
    case 0x02: wait = WaitMode::Or; break;
    default: LOG_CRIT(L"unknown mode: %u", waitMode);
  }

  switch (waitMode & 0xf0u) {
    case 0x00: clear = ClearMode::None; break;
    case 0x10: clear = ClearMode::All; break;
    case 0x20: clear = ClearMode::Bits; break;
    default: LOG_CRIT(L"unknown mode: %u", waitMode);
  }
  return ef->wait(bitPattern, wait, clear, pResultPat, pTimeout);
}

EXPORT SYSV_ABI int sceKernelPollEventFlag(IKernelEventFlag_t ef, uint64_t bitPattern, uint32_t waitMode, uint64_t* pResultPat) {
  LOG_USE_MODULE(eventFlag);

  if (ef == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  if (bitPattern == 0) {
    return getErr(ErrCode::_EINVAL);
  }

  WaitMode  wait  = WaitMode::And;
  ClearMode clear = ClearMode::None;

  switch (waitMode & 0xfu) {
    case 0x01: wait = WaitMode::And; break;
    case 0x02: wait = WaitMode::Or; break;
    default: LOG_CRIT(L"unknown mode: %u", waitMode);
  }

  switch (waitMode & 0xf0u) {
    case 0x00: clear = ClearMode::None; break;
    case 0x10: clear = ClearMode::All; break;
    case 0x20: clear = ClearMode::Bits; break;
    default: LOG_CRIT(L"unknown mode: %u", waitMode);
  }

  return ef->poll(bitPattern, wait, clear, pResultPat);
}

EXPORT SYSV_ABI int sceKernelSetEventFlag(IKernelEventFlag_t ef, uint64_t bitPattern) {
  if (ef == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  ef->set(bitPattern);

  return Ok;
}

EXPORT SYSV_ABI int sceKernelClearEventFlag(IKernelEventFlag_t ef, uint64_t bitPattern) {
  if (ef == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  ef->clear(bitPattern);

  return Ok;
}

EXPORT SYSV_ABI int sceKernelCancelEventFlag(IKernelEventFlag_t ef, uint64_t setPattern, int* pNumWaitThreads) {
  if (ef == nullptr) {
    return getErr(ErrCode::_ESRCH);
  }

  ef->cancel(setPattern, pNumWaitThreads);

  return Ok;
}
}