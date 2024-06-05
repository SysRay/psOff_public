#pragma once

#include "utility/utility.h"

#include <functional>

class ICommunication {
  CLASS_NO_COPY(ICommunication);
  CLASS_NO_MOVE(ICommunication);

  public:
  enum class PacketID : uint16_t {
    IPC_PACKET_HANDSHAKE,
  };

  struct HandshakePacket {
    PacketID   packetId;
    const char data[16];
  };

  union UPackets {
    PacketID        packetId;
    HandshakePacket hshake;
  };

  typedef std::function<void(UPackets* packet)> PHandler;

  ICommunication()          = default;
  virtual ~ICommunication() = default;

  virtual bool init(const char* name)  = 0;
  virtual bool deinit()                = 0;
  virtual bool write(UPackets* packet) = 0;
  virtual void runReadLoop()           = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL ICommunication& accessIPC();
#undef __APICALL
