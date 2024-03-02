#pragma once
#include "utility/utility.h"

class ITimer {
  CLASS_NO_COPY(ITimer);

  protected:
  ITimer() = default;

  public:
  /**
   * @brief init the timer. Sets start time for relative time
   *
   */
  virtual void init() = 0;

  /**
   * @brief Get the relative ticks
   *
   * @return uint64_t
   */
  virtual uint64_t getTicks() = 0;

  /**
   * @brief Get the relative Time in secounds
   *
   * @return double
   */
  virtual double getTimeS() = 0;

  /**
   * @brief Get the relative Time in ms
   *
   * @return double
   */
  virtual double getTimeMs() = 0;

  /**
   * @brief  Get the absolute ticks
   *
   * @return uint64_t
   */
  virtual uint64_t queryPerformance() = 0;

  /**
   * @brief Pauses the relative timer
   *
   */
  virtual void pause() = 0;

  /**
   * @brief Resumes the relative timer
   *
   */
  virtual void resume() = 0;

  /**
   * @brief Get the Frequency of the ticks
   *
   * @return uint64_t
   */
  virtual uint64_t getFrequency() = 0;

  /**
   * @brief Check if paused
   *
   * @return true
   * @return false
   */
  virtual bool getPaused() = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL ITimer& accessTimer();
#undef __APICALL