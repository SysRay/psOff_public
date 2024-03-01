#define __APICALL_EXTERN
#include "timer.h"
#undef __APICALL_EXTERN

#include "SysWindowsTimer.h" // todo use boost

class Timer: public ITimer {
  uint64_t m_startTime;
  uint64_t m_PauseTime = 1;
  uint64_t m_freq      = 0;

  public:
  Timer() = default;

  void init() override {
    System::sys_query_performance_counter(&m_startTime);
    m_PauseTime = 0;
    System::sys_query_performance_frequency(&m_freq);
  }

  double getTimeS() override {
    if (m_PauseTime > 0) {
      return (static_cast<double>(m_PauseTime - m_startTime)) / static_cast<double>(m_freq);
    }

    uint64_t currentTime = 0;
    System::sys_query_performance_counter(&currentTime);

    return (static_cast<double>(currentTime - m_startTime)) / static_cast<double>(m_freq);
  }

  double getTimeMs() override { return 1e3 * getTimeS(); }

  uint64_t getTicks() override {
    if (m_PauseTime > 0) {
      return (m_PauseTime - m_startTime);
    }

    uint64_t currentTime = 0;
    System::sys_query_performance_counter(&currentTime);
    return (currentTime - m_startTime);
  }

  uint64_t queryPerformance() override {
    uint64_t ret = 0;
    System::sys_query_performance_counter(&ret);
    return ret;
  }

  void pause() override { System::sys_query_performance_counter(&m_PauseTime); }

  void resume() override {

    uint64_t currentTime = 0;
    System::sys_query_performance_counter(&currentTime);

    m_startTime += currentTime - m_PauseTime;
    m_PauseTime = 0;
  }

  uint64_t getFrequency() override { return m_freq; }

  bool getPaused() override { return m_PauseTime > 0; }
};

ITimer& accessTimer() {
  static Timer inst;
  return inst;
}