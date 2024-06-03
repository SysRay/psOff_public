#pragma once
#include "codes.h"

typedef int     SceNetSocklen_t;
typedef uint8_t SceNetSaFamily_t;

struct SceNetSockaddr {
  uint8_t          sa_len;
  SceNetSaFamily_t sa_family;
  char             sa_data[14];
};

typedef int* SceRudpEventHandler(int eventId, int soc, uint8_t const* data, size_t dataLen, struct SceNetSockaddr const* addr, SceNetSocklen_t addrLen,
                                 void* arg);

typedef void* SceRudpContextEventHandler(int ctxId, int eventId, int errorCode, void* arg);

struct SceRudpContextStatus {
  uint32_t state;
  int      parentId;
  uint32_t children;
  uint32_t lostPackets;
  uint32_t sentPackets;
  uint32_t rcvdPackets;
  uint64_t sentBytes;
  uint64_t rcvdBytes;
  uint32_t retransmissions;
  uint32_t rtt;
};

struct SceRudpStatus {
  uint64_t sentUdpBytes;
  uint64_t rcvdUdpBytes;
  uint32_t sentUdpPackets;
  uint32_t rcvdUdpPackets;
  uint64_t sentUserBytes;
  uint32_t sentUserPackets;
  uint64_t rcvdUserBytes;
  uint32_t rcvdUserPackets;
  uint32_t sentLatencyCriticalPackets;
  uint32_t rcvdLatencyCriticalPackets;
  uint32_t sentSynPackets;
  uint32_t rcvdSynPackets;
  uint32_t sentUsrPackets;
  uint32_t rcvdUsrPackets;
  uint32_t sentPrbPackets;
  uint32_t rcvdPrbPackets;
  uint32_t sentRstPackets;
  uint32_t rcvdRstPackets;
  uint32_t lostPackets;
  uint32_t retransmittedPackets;
  uint32_t reorderedPackets;
  uint32_t currentContexts;
  uint64_t sentQualityLevel1Bytes;
  uint64_t rcvdQualityLevel1Bytes;
  uint32_t sentQualityLevel1Packets;
  uint32_t rcvdQualityLevel1Packets;
  uint64_t sentQualityLevel2Bytes;
  uint64_t rcvdQualityLevel2Bytes;
  uint32_t sentQualityLevel2Packets;
  uint32_t rcvdQualityLevel2Packets;
  uint64_t sentQualityLevel3Bytes;
  uint64_t rcvdQualityLevel3Bytes;
  uint32_t sentQualityLevel3Packets;
  uint32_t rcvdQualityLevel3Packets;
  uint64_t sentQualityLevel4Bytes;
  uint64_t rcvdQualityLevel4Bytes;
  uint32_t sentQualityLevel4Packets;
  uint32_t rcvdQualityLevel4Packets;
  uint32_t allocs;
  uint32_t frees;
  uint32_t memCurrent;
  uint32_t memPeak;
  uint32_t establishedConnections;
  uint32_t failedConnections;
  uint32_t failedConnectionsReset;
  uint32_t failedConnectionsRefused;
  uint32_t failedConnectionsTimeout;
  uint32_t failedConnectionsVersionMismatch;
  uint32_t failedConnectionsTransportTypeMismatch;
  uint32_t failedConnectionsQualityLevelMismatch;
};

struct SceRudpReadInfo {
  uint8_t  size;
  uint8_t  retransmissionCount;
  uint16_t retransmissionDelay;
  uint8_t  retransmissionDelay2;
  uint8_t  flags;
  uint16_t sequenceNumber;
  uint32_t timestamp;
};

struct SceRudpPollEvent {
  int      ctxId;
  uint16_t reqEvents;
  uint16_t rtnEvents;
};