#define __APICALL_EXTERN
#include "timer.h"
#undef __APICALL_EXTERN

#include "SysWindowsTimer.h" // todo use boost

#include <boost/chrono.hpp>
#include <boost/thread.hpp>

namespace {
void time2timespec(SceKernelTimespec* ts, double sec) {
  ts->tv_sec  = static_cast<decltype(ts->tv_sec)>(sec);
  ts->tv_nsec = static_cast<decltype(ts->tv_nsec)>((sec - static_cast<double>(ts->tv_sec)) * 1e9);
}
} // namespace

class Timer: public ITimer {
  uint64_t m_startTime;
  uint64_t m_PauseTime = 1;
  uint64_t m_freq      = 0;

  public:
  Timer() = default;

  void init() final {
    System::sys_query_performance_counter(&m_startTime);
    m_PauseTime = 0;
    System::sys_query_performance_frequency(&m_freq);
  }

  double getTimeS() final {
    if (m_PauseTime > 0) {
      return (static_cast<double>(m_PauseTime - m_startTime)) / static_cast<double>(m_freq);
    }

    uint64_t currentTime = 0;
    System::sys_query_performance_counter(&currentTime);

    return (static_cast<double>(currentTime - m_startTime)) / static_cast<double>(m_freq);
  }

  double getTimeMs() final { return 1e3 * getTimeS(); }

  uint64_t getTicks() final {
    if (m_PauseTime > 0) {
      return (m_PauseTime - m_startTime);
    }

    uint64_t currentTime = 0;
    System::sys_query_performance_counter(&currentTime);
    return (currentTime - m_startTime);
  }

  uint64_t queryPerformance() final {
    uint64_t ret = 0;
    System::sys_query_performance_counter(&ret);
    return ret;
  }

  void pause() final { System::sys_query_performance_counter(&m_PauseTime); }

  void resume() final {

    uint64_t currentTime = 0;
    System::sys_query_performance_counter(&currentTime);

    m_startTime += currentTime - m_PauseTime;
    m_PauseTime = 0;
  }

  uint64_t getFrequency() final { return m_freq; }

  bool getPaused() final { return m_PauseTime > 0; }

  int getTime(SceKernelClockid id, SceKernelTimespec* tp) final;
  int getTimeRes(SceKernelClockid id, SceKernelTimespec* tp) final;
  int getTimeofDay(SceKernelTimeval* tp) final;
  int getTimeZone(SceKernelTimezone* tz) final;
};

ITimer& accessTimer() {
  static Timer inst;
  return inst;
}

int Timer::getTime(SceKernelClockid id, SceKernelTimespec* tp) {
  if (tp == nullptr) {
    return getErr(ErrCode::_EFAULT);
  }

  using namespace boost::chrono;
  auto func = [tp](auto const& now) {
    tp->tv_sec  = time_point_cast<seconds>(now).time_since_epoch().count();
    tp->tv_nsec = time_point_cast<nanoseconds>(now).time_since_epoch().count() % 1000000000;
    return Ok;
  };

  switch (id) {
    case 0:
    case 9:
    case 10: return func(system_clock::now());
    case 4:
    case 11:
    case 12:
    case 13: return func(steady_clock::now());
    case 1:
    case 2:
    case 5:
    case 7:
    case 8: {
      double const ts = accessTimer().getTimeS();
      time2timespec(tp, ts);
      return Ok;
    }
    case 14: return func(thread_clock::now());
    case 15: return func(process_cpu_clock::now());
  }
  return getErr(ErrCode::_EINVAL);
}

int Timer::getTimeRes(SceKernelClockid id, SceKernelTimespec* tp) {
  if (tp == nullptr) {
    return getErr(ErrCode::_EFAULT);
  }

  using namespace boost::chrono;
  auto func = [tp](auto const& period) {
    tp->tv_sec  = static_cast<long>(period.num / period.den);
    tp->tv_nsec = static_cast<long>((period.num % period.den) * 1e9 / period.den);
    return Ok;
  };

  switch (id) {
    case 0:
    case 9:
    case 10: return func(system_clock::period());
    case 4:
    case 11:
    case 12: return func(steady_clock::period());
    case 1:
    case 2:
    case 5:
    case 7:
    case 8: {
      double const ts = 1.0 / accessTimer().getFrequency();
      time2timespec(tp, ts);
      return Ok;
    }
    case 14: return func(thread_clock::period());
    case 15: return func(process_cpu_clock::period());
  }
  return getErr(ErrCode::_EINVAL);
}

int Timer::getTimeofDay(SceKernelTimeval* tp) {
  if (tp == nullptr) {
    return getErr(ErrCode::_EFAULT);
  }

  using namespace boost::chrono;
  uint64_t const t = time_point_cast<microseconds>(system_clock::now()).time_since_epoch().count();
  micro2timeval(tp, t);
  return Ok;
}

int Timer::getTimeZone(SceKernelTimezone* tz) {
  if (tz == nullptr) return getErr(ErrCode::_EINVAL);
  static bool isTZSet = false;

  if (!isTZSet) {
    _tzset();
    isTZSet = true;
  }

  long tz_secswest;
  if (auto err = _get_timezone(&tz_secswest)) return getErr((ErrCode)err);
  if (auto err = _get_daylight(&tz->tz_dsttime)) return getErr((ErrCode)err);
  tz->tz_minuteswest = int(tz_secswest / 60);

  return Ok;
}
