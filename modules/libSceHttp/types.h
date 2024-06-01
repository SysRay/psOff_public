#pragma once
#include "codes.h"
#include "utility/utility.h"

enum class SceHttpEvent : unsigned int {
  NB_EVENT_IN           = 0x00000001, // IN
  NB_EVENT_OUT          = 0x00000002, // OUT
  NB_EVENT_SOCK_ERR     = 0x00000008, // SOCK_ERROR
  NB_EVENT_HUP          = 0x00000010, // HUP
  NB_EVENT_ICM          = 0x00000020, // ICM Event
  NB_EVENT_RESOLVED     = 0x00010000, // Resolved
  NB_EVENT_RESOLVER_ERR = 0x00020000, // Resolver Error

};

enum classSceHttpHttpVersion { SCE_HTTP_VERSION_1_0 = 1, SCE_HTTP_VERSION_1_1 };

enum classSceHttpProxyMode { SCE_HTTP_PROXY_AUTO, SCE_HTTP_PROXY_MANUAL };

enum classSceHttpAddHeaderMode { SCE_HTTP_HEADER_OVERWRITE, SCE_HTTP_HEADER_ADD };

enum class SceHttpAuthType { SCE_HTTP_AUTH_BASIC, SCE_HTTP_AUTH_DIGEST, SCE_HTTP_AUTH_RESERVED0, SCE_HTTP_AUTH_RESERVED1, SCE_HTTP_AUTH_RESERVED2 };

enum class SceHttpContentLengthType { SCE_HTTP_CONTENTLEN_EXIST, SCE_HTTP_CONTENTLEN_NOT_FOUND, SCE_HTTP_CONTENTLEN_CHUNK_ENC };

enum class SceHttpRequestStatus {
  SCE_HTTP_REQUEST_STATUS_CONNECTION_RESERVED,
  SCE_HTTP_REQUEST_STATUS_RESOLVING_NAME,
  SCE_HTTP_REQUEST_STATUS_NAME_RESOLVED,
  SCE_HTTP_REQUEST_STATUS_CONNECTING,
  SCE_HTTP_REQUEST_STATUS_CONNECTED,
  SCE_HTTP_REQUEST_STATUS_TLS_CONNECTING,
  SCE_HTTP_REQUEST_STATUS_TLS_CONNECTED,
  SCE_HTTP_REQUEST_STATUS_SENDING_REQUEST,
  SCE_HTTP_REQUEST_STATUS_REQUEST_SENT,
  SCE_HTTP_REQUEST_STATUS_RECEIVING_HEADER,
  SCE_HTTP_REQUEST_STATUS_HEADER_RECEIVED,
};

using SceHttpAuthInfoCallback = SYSV_ABI int (*)(int request, SceHttpAuthType authType, const char* realm, char* username, char* password, int isNeedEntity,
                                                 uint8_t** entityBody, size_t* entitySize, int* isSave, void* userArg);

using SceHttpRedirectCallback = SYSV_ABI int (*)(int request, int32_t statusCode, int32_t* method, const char* location, void* userArg);

using SceHttpRequestStatusCallback = SYSV_ABI void (*)(int request, SceHttpRequestStatus requestStatus, void* userArg);

typedef void* SceHttpEpollHandle;

struct SceHttpMemoryPoolStats {
  size_t poolSize;
  size_t maxInuseSize;
  size_t currentInuseSize;
};

struct SceHttpNBEvent {
  uint32_t events;
  uint32_t eventDetail;
  int      id;
  void*    userArg;
};
