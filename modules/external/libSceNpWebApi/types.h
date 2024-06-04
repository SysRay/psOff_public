#pragma once
#include "codes.h"

enum class SceNpWebApiHttpMethod { GET, POST, PUT, DELETE };

struct SceNpWebApiMemoryPoolStats {
  size_t poolSize;         /*E Total memory size */
  size_t maxInuseSize;     /*E Max memory usage */
  size_t currentInuseSize; /*E Current memory usage */
};

struct SceNpWebApiResponseInformationOption {
  int32_t httpStatus;
  char*   pErrorObject;
  size_t  errorObjectSize;
  size_t  responseDataSize;
};

struct SceNpWebApiConnectionStats {
  uint32_t max;
  uint32_t used;
  uint32_t unused;
  uint32_t keepAlive;
};

struct SceNpWebApiContentParameter {
  size_t      contentLength;
  const char* pContentType;
  uint8_t     reserved[16];
};

struct SceNpWebApiHttpHeader {
  char* pName;
  char* pValue;
};

struct SceNpWebApiMultipartPartParameter {
  SceNpWebApiHttpHeader* pHeaders;
  size_t                 headerNum;
  size_t                 contentLength;
};

/*
 * Extended Push Event
 */

struct SceNpWebApiPushEventDataType {
  char val[SCE_NP_WEBAPI_PUSH_EVENT_DATA_TYPE_LEN_MAX + 1];
};

struct SceNpWebApiExtdPushEventExtdDataKey {
  char val[SCE_NP_WEBAPI_EXTD_PUSH_EVENT_EXTD_DATA_KEY_LEN_MAX + 1];
};

struct SceNpWebApiExtdPushEventFilterParameter {
  SceNpWebApiPushEventDataType         dataType;
  SceNpWebApiExtdPushEventExtdDataKey* pExtdDataKey;
  size_t                               extdDataKeyNum;
};

struct SceNpWebApiExtdPushEventExtdData {
  SceNpWebApiExtdPushEventExtdDataKey extdDataKey;
  char*                               pData;
  size_t                              dataLen;
};
