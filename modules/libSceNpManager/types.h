#pragma once
#include "codes.h"
#include "onlineId.h"

typedef uint64_t SceNpGameCustomDataId;
typedef int32_t  SceNpSessionInvitationEventFlag;

typedef int32_t SceNpPlusEventType;

typedef uint64_t SceNpAccountId;

enum class SceNpPlatformType : int {
  TYPE_NONE  = 0,
  TYPE_PS3   = 1,
  TYPE_VITA  = 2,
  TYPE_PS4   = 3,
  TYPE_ORBIS = 3 /* for compatibility */
};

enum class SceNpState { UNKNOWN = 0, SIGNED_OUT, SIGNED_IN };
enum class SceNpGamePresenceStatus { OFFLINE, ONLINE };
enum class SceNpReachabilityState { UNAVAILABLE = 0, AVAILABLE, REACHABLE };
enum class SceNpServiceLabel : uint32_t { DEFAULT = 0x00000000, INVALID = 0xFFFFFFFF };

struct SceNpTitleId {
  char id[SCE_NP_TITLE_ID_LEN + 1];
};

struct SceNpTitleSecret {
  uint8_t data[SCE_NP_TITLE_SECRET_SIZE];
};

struct SceNpClientId {
  char id[SCE_NP_CLIENT_ID_MAX_LEN + 1];
};

struct SceNpClientSecret {
  char secret[SCE_NP_CLIENT_SECRET_MAX_LEN + 1];
};

struct SceNpAuthorizationCode {
  char code[SCE_NP_AUTHORIZATION_CODE_MAX_LEN + 1];
};

struct SceNpIdToken {
  char token[SCE_NP_ID_TOKEN_MAX_LEN + 1];
};

struct SceNpCommunicationId {
  char    data[9];
  char    term;
  uint8_t num;
  char    dummy;
};

struct SceNpCommunicationPassphrase {
  uint8_t data[SCE_NP_COMMUNICATION_PASSPHRASE_SIZE];
};

struct SceNpUnifiedEntitlementLabel {
  char data[SCE_NP_UNIFIED_ENTITLEMENT_LABEL_SIZE];
};

struct SceNpPeerAddressA {
  SceNpAccountId    accountId;
  SceNpPlatformType platform;
};

struct SceNpLanguageCode {
  char code[SCE_NP_LANGUAGE_CODE_MAX_LEN + 1];
};

struct SceNpCountryCode {
  char data[SCE_NP_COUNTRY_CODE_LENGTH];
  char term;
};

struct SceNpDate {
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
};

struct SceNpAgeRestriction {
  SceNpCountryCode countryCode;
  int8_t           age;
};

struct SceNpContentRestriction {
  size_t size;
  int8_t defaultAgeRestriction;

  char    padding[3];
  int32_t ageRestrictionCount;

  const SceNpAgeRestriction* ageRestriction;
};

struct SceNpParentalControlInfo {
  bool contentRestriction;
  bool chatRestriction;
  bool ugcRestriction;
};

struct SceNpCheckPlusParameter {
  size_t   size;
  int32_t  userId;
  char     padding[4];
  uint64_t features;
};

struct SceNpCheckPlusResult {
  bool authorized;
};

struct SceNpNotifyPlusFeatureParameter {
  size_t   size;
  int32_t  userId;
  char     padding[4];
  uint64_t features;
};

struct SceNpSessionId {
  char data[SCE_NP_SESSION_ID_MAX_SIZE];
  char term;
};

struct SceNpInvitationId {
  char data[SCE_NP_INVITATION_ID_MAX_SIZE];
  char term;
};

struct SceNpSessionInvitationEventParam {
  SceNpSessionId                  sessionId;
  SceNpInvitationId               invitationId;
  SceNpSessionInvitationEventFlag flag;
  char                            padding[4];
  SceNpOnlineId                   onlineId;
  int32_t                         userId;
  SceNpOnlineId                   referralOnlineId;
  SceNpAccountId                  referralAccountId;
};

struct SceNpGameCustomDataEventParam {
  SceNpGameCustomDataId itemId;
  SceNpOnlineId         onlineId;
  int32_t               userId;
};

struct SceNpCreateAsyncRequestParameter {
  size_t   size;
  uint64_t cpuAffinityMask;
  int      threadPriority;
};
