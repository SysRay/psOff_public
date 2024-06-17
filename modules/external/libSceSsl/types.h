#pragma once
#include "codes.h"

enum class SceSslVersion : int {
  VERSION_NONE = 0,
  VERSION_SSL2_0,
  VERSION_SSL3_0,
  VERSION_TLS1_0,
  VERSION_TLS1_1,
  VERSION_TLS1_2

};
enum class SceSslErrorVerify : unsigned int {
  ERROR_VERIFY_INTERNAL         = 0x01U,
  ERROR_VERIFY_INVALID_CERT     = 0x02U,
  ERROR_VERIFY_CN_CHECK         = 0x04U,
  ERROR_VERify_NOT_AFTER_CHECK  = 0x08U,
  ERROR_VERify_NOT_BEFORE_CHECK = 0x10U,
  ERROR_VERIFY_UNKNOWN_CA       = 0x20U

};

struct SceSslMemoryPoolStats {
  size_t  poolSize;
  size_t  maxInuseSize;
  size_t  currentInuseSize;
  int32_t reserved;
};

typedef void SceSslCert;
typedef void SceSslCertName;

struct SceSslCaList {
  SceSslCert** caCerts;
  int          caNum;
};

struct SceSslData {
  char*  ptr;
  size_t size;
};

struct SceSslCaCerts {
  SceSslData* certData;
  size_t      certDataNum;
  void*       pool;
};