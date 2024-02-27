#pragma once

enum class SceHttpsFlags : unsigned int {
  SERVER_VERIFY    = 0x01U, // Server Certificate Verify
  CLIENT_VERIFY    = 0x02U, // Client Certificate Verify
  CN_CHECK         = 0x04U, // Common Name Check
  NOT_AFTER_CHECK  = 0x08U, // Not Valid After Check
  NOT_BEFORE_CHECK = 0x10U, // Not Valid Before Check
  KNOWN_CA_CHECK   = 0x20U, // Known CA Check
  SESSION_REUSE    = 0x40U, // Session Reuse
  SNI              = 0x80U  // Server Name Indication (SNI)

};

enum class SceHttpSslVersion { SSLV23, SSLV2, SSLV3, TLSV1 };