#include "codes.h"
#include "common.h"
#include "config_emu.h"
#include "logging.h"

#include <unordered_map>

LOG_DEFINE_MODULE(libSceUserService);

namespace {
struct UserServiceLoginUserIdList {
  int userId[4];
};

enum UserServiceEventType { UserServiceEventTypeLogin, UserServiceEventTypeLogout };

struct UserServiceEvent {
  UserServiceEventType eventType;
  int                  userId;
};

enum UserServiceUserColor { USER_COLOR_BLUE, USER_COLOR_RED, USER_COLOR_GREEN, USER_COLOR_PINK };

static std::unordered_map<std::string_view, UserServiceUserColor> colors = {
    {"blue", USER_COLOR_BLUE},
    {"red", USER_COLOR_RED},
    {"green", USER_COLOR_GREEN},
    {"pink", USER_COLOR_PINK},
};

static UserServiceUserColor map_user_color(std::string_view str) {
  auto it = colors.find(str);
  if (it != colors.end()) return it->second;
  return USER_COLOR_BLUE;
}

} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceUserService";

EXPORT SYSV_ABI int sceUserServiceInitialize(const void* /*params*/) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUserServiceInitialize2(int threadPriority, uint64_t cpuAffinityMask) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUserServiceTerminate() {
  return Ok;
}

EXPORT SYSV_ABI int sceUserServiceGetInitialUser(int* userId) {
  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);
  if (!getJsonParam(jData, "userIndex", *userId) || (*userId < 1 || *userId > 3)) *userId = 1;
  return Ok;
}

EXPORT SYSV_ABI int sceUserServiceGetEvent(UserServiceEvent* event) {
  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);

  int onlineUsers;
  if (getJsonParam(jData, "onlineUsers", onlineUsers)) {
    static int logins = 0;
    if (logins < std::max(1, std::min(onlineUsers, 4))) {
      event->eventType = UserServiceEventTypeLogin;
      event->userId    = ++logins;
      return Ok;
    }
  }

  return Err::UserService::NO_EVENT;
}

EXPORT SYSV_ABI int sceUserServiceGetLoginUserIdList(UserServiceLoginUserIdList* userId_list) {
  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);

  int onlineUsers;
  if (getJsonParam(jData, "onlineUsers", onlineUsers)) {
    onlineUsers = std::max(1, std::min(onlineUsers, 4));
    for (int i = 0; i < 4; i++) {
      userId_list->userId[i] = onlineUsers > i ? (i + 1) : -1;
    }
  }

  return 0;
}

EXPORT SYSV_ABI int sceUserServiceGetUserName(int userId, char* name, size_t size) {
  if (userId < 1 || userId > 3 || name == nullptr || size == 0) return Err::UserService::INVALID_ARGUMENT;

  std::string username = "Anon";
  auto [lock, jData]   = accessConfig()->accessModule(ConfigModFlag::GENERAL);

  try {
    auto& profiles = (*jData)["profiles"];
    auto& cprofile = profiles[userId - 1];
    cprofile["name"].get_to(username);
  } catch (json::exception& ex) {
  }

  if (size < (username.size() + 1)) return Err::UserService::BUFFER_TOO_SHORT;
  auto const count = username.copy(name, size - 1);
  name[count]      = '\0';
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUserServiceGetUserColor(int userId, UserServiceUserColor* color) {
  if (userId < 1 || userId > 3) return Err::UserService::INVALID_ARGUMENT;
  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);

  std::string _scolor;

  try {
    auto& profiles = (*jData)["profiles"];
    auto& cprofile = profiles[userId - 1];
    cprofile["color"].get_to(_scolor);
  } catch (json::exception& ex) {
  }

  *color = map_user_color(_scolor);
  return Ok;
}
}
