#include "common.h"
#include "errorcodes.h"
#include "logging.h"

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
  *userId = 1;

  return Ok;
}

EXPORT SYSV_ABI int sceUserServiceGetEvent(UserServiceEvent* event) {
  static bool logged_in = false;

  if (!logged_in) {
    logged_in        = true;
    event->eventType = UserServiceEventTypeLogin;
    event->userId    = 1;
    return Ok;
  }

  return Err::USER_SERVICE_ERROR_NO_EVENT;
}

EXPORT SYSV_ABI int sceUserServiceGetLoginUserIdList(UserServiceLoginUserIdList* userId_list) {
  userId_list->userId[0] = 1;
  userId_list->userId[1] = -1;
  userId_list->userId[2] = -1;
  userId_list->userId[3] = -1;

  return 0;
}

EXPORT SYSV_ABI int sceUserServiceGetUserName(int userId, char* name, size_t size) {
  std::string username = "Anon";
  auto const  count    = username.copy(name, size - 1);
  name[count]          = '\0';
  return Ok;
}

EXPORT SYSV_ABI int32_t sceUserServiceGetUserColor(int userId, UserServiceUserColor* color) {
  *color = USER_COLOR_BLUE;
  return Ok;
}
}