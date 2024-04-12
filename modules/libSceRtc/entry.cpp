#include "common.h"
#include "logging.h"
#include "types.h"

#include <boost/chrono.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>

LOG_DEFINE_MODULE(libSceRtc);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceRtc";

EXPORT SYSV_ABI int sceRtcGetCurrentTick(SceRtcTick* pTick) {
  pTick->tick = boost::chrono::duration_cast<boost::chrono::nanoseconds>(boost::chrono::high_resolution_clock::now().time_since_epoch()).count();
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetCurrentClock(SceRtcDateTime* pTime, int iTimeZone) {
  boost::posix_time::ptime const now = boost::posix_time::microsec_clock::universal_time();

  boost::posix_time::time_duration const duration = now.time_of_day() + boost::posix_time::minutes(iTimeZone);

  pTime->year        = now.date().year();
  pTime->month       = now.date().month();
  pTime->day         = now.date().day();
  pTime->hour        = duration.hours();
  pTime->minute      = duration.minutes();
  pTime->second      = duration.seconds();
  pTime->microsecond = duration.fractional_seconds() * 1000;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetCurrentClockLocalTime(SceRtcDateTime* pTime) {
  boost::posix_time::ptime const now = boost::posix_time::microsec_clock::local_time();

  boost::posix_time::time_duration const duration = now.time_of_day();

  pTime->year        = now.date().year();
  pTime->month       = now.date().month();
  pTime->day         = now.date().day();
  pTime->hour        = duration.hours();
  pTime->minute      = duration.minutes();
  pTime->second      = duration.seconds();
  pTime->microsecond = duration.fractional_seconds() * 1000;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetCurrentNetworkTick(SceRtcTick* pTick) {
  pTick->tick = boost::chrono::system_clock::now().time_since_epoch().count();
  return Ok;
}

EXPORT SYSV_ABI int sceRtcConvertUtcToLocalTime(const SceRtcTick* pUtc, SceRtcTick* pLocalTime) {
  *pLocalTime = *pUtc;

  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcConvertLocalTimeToUtc(const SceRtcTick* pLocalTime, SceRtcTick* pUtc) {
  *pUtc = *pLocalTime;

  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcIsLeapYear(int year) {
  return boost::gregorian::gregorian_calendar::is_leap_year(2020);
}

EXPORT SYSV_ABI int sceRtcGetDaysInMonth(int year, int month) {
  return boost::gregorian::gregorian_calendar::end_of_month_day(year, month);
}

EXPORT SYSV_ABI SceRTCDay sceRtcGetDayOfWeek(int year, int month, int day) {
  boost::gregorian::date const date(year, month, day);

  return (SceRTCDay)date.day_of_week().as_number();
}

EXPORT SYSV_ABI int sceRtcCheckValid(const SceRtcDateTime* pTime) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcSetTime_t(SceRtcDateTime* pTime, time_t llTime) {
  boost::posix_time::ptime const ptime = boost::posix_time::from_time_t(llTime);

  auto const date = ptime.date();
  auto const time = ptime.time_of_day();

  pTime->year   = date.year();
  pTime->month  = date.month();
  pTime->day    = date.day();
  pTime->hour   = time.hours();
  pTime->minute = time.minutes();
  pTime->second = time.seconds();
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetTime_t(const SceRtcDateTime* pTime, time_t* pllTime) {
  boost::posix_time::ptime ptime(boost::gregorian::date(pTime->year, pTime->month, pTime->day),
                                 boost::posix_time::seconds(pTime->hour * 3600 + pTime->minute * 60 + pTime->second));

  *pllTime = boost::posix_time::to_time_t(ptime);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcSetDosTime(SceRtcDateTime* pTime, unsigned int uiDosTime) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetDosTime(const SceRtcDateTime* pTime, unsigned int* puiDosTime) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcSetWin32FileTime(SceRtcDateTime* pTime, uint64_t ulWin32Time) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetWin32FileTime(const SceRtcDateTime* pTime, uint64_t* ulWin32Time) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcSetTick(SceRtcDateTime* pTime, const SceRtcTick* pTick) {
  boost::posix_time::ptime const ptime = boost::posix_time::from_time_t(pTick->tick);

  auto const date = ptime.date();
  auto const time = ptime.time_of_day();

  pTime->year   = date.year();
  pTime->month  = date.month();
  pTime->day    = date.day();
  pTime->hour   = time.hours();
  pTime->minute = time.minutes();
  pTime->second = time.seconds();
  return Ok;
}

EXPORT SYSV_ABI int sceRtcGetTick(const SceRtcDateTime* pTime, SceRtcTick* pTick) {
  boost::posix_time::ptime ptime(boost::gregorian::date(pTime->year, pTime->month, pTime->day),
                                 boost::posix_time::seconds(pTime->hour * 3600 + pTime->minute * 60 + pTime->second));

  pTick->tick = boost::posix_time::to_time_t(ptime);
  return Ok;
}

EXPORT SYSV_ABI unsigned int sceRtcGetTickResolution(void) {
  return 1000000;
}

EXPORT SYSV_ABI int sceRtcTickAddTicks(SceRtcTick* pTick0, const SceRtcTick* pTick1, int64_t lAdd) {
  pTick0->tick = pTick1->tick + lAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddMicroseconds(SceRtcTick* pTick0, const SceRtcTick* pTick1, int64_t lAdd) {
  pTick0->tick = pTick1->tick + lAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddSeconds(SceRtcTick* pTick0, const SceRtcTick* pTick1, int64_t lAdd) {
  pTick0->tick = uint64_t(1e6) * pTick1->tick + lAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddMinutes(SceRtcTick* pTick0, const SceRtcTick* pTick1, int64_t lAdd) {
  pTick0->tick = uint64_t(60 * 1e6) * pTick1->tick + lAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddHours(SceRtcTick* pTick0, const SceRtcTick* pTick1, int iAdd) {
  pTick0->tick = uint64_t(60 * 60 * 1e6) * pTick1->tick + iAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddDays(SceRtcTick* pTick0, const SceRtcTick* pTick1, int iAdd) {
  pTick0->tick = uint64_t(24 * 60 * 60 * 1e6) * pTick1->tick + iAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddWeeks(SceRtcTick* pTick0, const SceRtcTick* pTick1, int iAdd) {
  pTick0->tick = uint64_t(7 * 60 * 60 * 1e6) * pTick1->tick + iAdd;
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddMonths(SceRtcTick* pTick0, const SceRtcTick* pTick1, int iAdd) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcTickAddYears(SceRtcTick* pTick0, const SceRtcTick* pTick1, int iAdd) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcFormatRFC2822(char* pszDateTime, const SceRtcTick* pUtc, int iTimeZoneMinutes) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcFormatRFC2822LocalTime(char* pszDateTime, const SceRtcTick* pUtc) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcFormatRFC3339(char* pszDateTime, const SceRtcTick* pUtc, int iTimeZoneMinutes) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcFormatRFC3339LocalTime(char* pszDateTime, const SceRtcTick* pUtc) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcParseDateTime(SceRtcTick* pUtc, const char* pszDateTime) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRtcParseRFC3339(SceRtcTick* pUtc, const char* pszDateTime) {
  LOG_USE_MODULE(libSceRtc);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
