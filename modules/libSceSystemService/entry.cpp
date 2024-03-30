#include "common.h"
#include "logging.h"
#include "system_param.h"
#include "tools/config_emu/config_emu.h"
#include "types.h"

#include <chrono>

LOG_DEFINE_MODULE(libSceSystemService);

namespace {
int getTimeZoneOffset() {
  auto const                    current_time = std::chrono::system_clock::now();
  const std::chrono::zoned_time local_time {std::chrono::current_zone(), current_time};
  return local_time.get_info().offset.count();
}

bool isDaylightTime() {
  auto const                    current_time = std::chrono::system_clock::now();
  const std::chrono::zoned_time local_time {std::chrono::current_zone(), current_time};
  using namespace std::chrono_literals;
  return local_time.get_info().save != 0min;
}

bool is24HourFormat() {

  auto                          current_time = std::chrono::system_clock::now();
  const std::chrono::zoned_time local_time {std::chrono::current_zone(), current_time};

  std::time_t time = std::chrono::system_clock::to_time_t(local_time);
  struct tm   info;
  if (auto err = localtime_s(&info, &time); err != NULL) {
    LOG_USE_MODULE(libSceSystemService);
    LOG_CRIT(L"localtime_s %d", err);
    return false;
  }

  // Check if the hours value is less than 13
  return info.tm_hour >= 13;
}

SystemParamDateFormat getDateFormat() {

  std::time_get<char>::dateorder order = std::use_facet<std::time_get<char>>(std::locale("")).date_order();
  switch (order) {
    case std::time_get<char>::dmy: return SystemParamDateFormat::DDMMYYYY;
    case std::time_get<char>::mdy: return SystemParamDateFormat::MMDDYYYY;
    case std::time_get<char>::ymd: return SystemParamDateFormat::YYYYMMDD;
    case std::time_get<char>::ydm: return SystemParamDateFormat::YYYYMMDD;
    default: return SystemParamDateFormat::DDMMYYYY; // case std::time_get<char>::no_order
  }
}

SystemParamLang getSystemLanguage() {
  LOG_USE_MODULE(libSceSystemService);
  SystemParamLang systemlang = SystemParamLang::EnglishUS;
  {
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);

    try {
      (*jData)["systemlang"].get_to(systemlang);
    } catch (const json::exception& e) {
      LOG_ERR(L"Invalid system language setting: %S", e.what());
    }
  }
  return systemlang;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSystemService";

EXPORT SYSV_ABI int32_t sceSystemServiceParamGetInt(SceSystemServiceParamId paramId, int32_t* value) {
  LOG_USE_MODULE(libSceSystemService);
  if (value == nullptr) {
    return Err::SERVICE_ERROR_PARAMETER;
  }

  switch (paramId) {
    case SceSystemServiceParamId::Lang: *value = (int)getSystemLanguage(); break;
    case SceSystemServiceParamId::DateFormat: *value = (int)getDateFormat(); break;
    case SceSystemServiceParamId::TimeFormat: *value = (int)is24HourFormat(); break;
    case SceSystemServiceParamId::TimeZone: *value = getTimeZoneOffset(); break;
    case SceSystemServiceParamId::Summertime: *value = isDaylightTime(); break;
    case SceSystemServiceParamId::SystemName: *value = 0; break;
    case SceSystemServiceParamId::GameParentalLevel: *value = (int)SystemParamGameParental::OFF; break;
    case SceSystemServiceParamId::ENTER_BUTTON_ASSIGN: *value = (int)SystemParamEnterButtonAssign::CROSS; break;
  }

  LOG_INFO(L" %d = %d", paramId, *value);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceParamGetString(SceSystemServiceParamId paramId, char* buf, size_t bufSize) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceReceiveEvent(SceSystemServiceEvent* event) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceGetStatus(SceSystemServiceStatus* status) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  if (status == nullptr) {
    return Err::SERVICE_ERROR_PARAMETER;
  }

  *status = SceSystemServiceStatus();

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceLoadExec(const char* path, char* const argv[]) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceHideSplashScreen() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceDisableMusicPlayer() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceReenableMusicPlayer() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceSetGpuLoadEmulationMode(SceSystemServiceGpuLoadEmulationMode mode) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI SceSystemServiceGpuLoadEmulationMode sceSystemServiceGetGpuLoadEmulationMode() {
  return SceSystemServiceGpuLoadEmulationMode::NORMAL;
}

EXPORT SYSV_ABI int32_t sceSystemServiceGetDisplaySafeAreaInfo(SceSystemServiceDisplaySafeAreaInfo* info) {
  if (info == nullptr) {
    return Err::SERVICE_ERROR_PARAMETER;
  }

  info->ratio = 16.0f / 9.0f;

  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceLaunchWebBrowser(const char* uri, const SceSystemServiceLaunchWebBrowserParam* param) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceLaunchEventDetails(int32_t userId, const char* eventId) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceLaunchTournamentsTeamProfile(int32_t userId, const char* teamId, const char* eventId, const char* teamPlatform) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceLaunchTournamentList(int32_t userId) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServicePowerTick() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceEnableSuspendConfirmationDialog() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceDisableSuspendConfirmationDialog() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

typedef struct SceSystemServiceAbnormalTerminationInfo SceSystemServiceAbnormalTerminationInfo;

EXPORT SYSV_ABI int32_t sceSystemServiceReportAbnormalTermination(const SceSystemServiceAbnormalTerminationInfo* info) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceIsEyeToEyeDistanceAdjusted(int32_t userId, bool* isAdjusted) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceShowEyeToEyeDistanceSetting() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceDisablePersonalEyeToEyeDistanceSetting() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceEnablePersonalEyeToEyeDistanceSetting() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceShowControllerSettings() {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceSystemServiceGetHdrToneMapLuminance(SceSystemServiceHdrToneMapLuminance* hdrToneMapLuminance) {
  LOG_USE_MODULE(libSceSystemService);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}