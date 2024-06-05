#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSsl);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSsl";

EXPORT SYSV_ABI int sceSslInit(size_t poolSize) {
  static int id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceSslTerm(int libsslCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetMemoryPoolStats(int libsslCtxId, SceSslMemoryPoolStats* currentStat) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetCaList(int libsslCtxId, SceSslCaList* caList) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslFreeCaList(int libsslCtxId, SceSslCaList* caList) {
  return Ok;
}

EXPORT SYSV_ABI SceSslCertName* sceSslGetSubjectName(int libsslCtxId, SceSslCert* sslCert) {
  return nullptr;
}

SceSslCertName* sceSslGetIssuerName(int libsslCtxId, SceSslCert* sslCert) {
  return nullptr;
}

EXPORT SYSV_ABI int sceSslGetNotAfter(int libsslCtxId, SceSslCert* sslCert, uint64_t* limit) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetNotBefore(int libsslCtxId, SceSslCert* sslCert, uint64_t* begin) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetNameEntryCount(int libsslCtxId, const SceSslCertName* certName) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetNameEntryInfo(int libsslCtxId, const SceSslCertName* certName, int entryNum, char* oidname, size_t maxOidnameLen, uint8_t* value,
                                           size_t maxValueLen, size_t* valueLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslFreeSslCertName(int libsslCtxId, SceSslCertName* certName) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetSerialNumber(int libsslCtxId, SceSslCert* sslCert, uint8_t* sboData, size_t* sboLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceSslGetCaCerts(int libsslCtxId, SceSslCaCerts* caCerts) {
  return Ok;
}
}