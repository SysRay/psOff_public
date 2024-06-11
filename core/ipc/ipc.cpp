#define __APICALL_EXTERN
#include "ipc.h"
#undef __APICALL_EXTERN

#include <Windows.h>
#include <boost/interprocess/ipc/message_queue.hpp>

class Communication: public ICommunication {
  private:
  boost::interprocess::message_queue mq;
  std::vector<PHandler>              m_handlers = {};

  public:
  Communication(const char* name): mq(boost::interprocess::open_only, name) {}

  void write(IPCHeader* packet) final { mq.send((char*)packet, packet->bodySize + sizeof(IPCHeader), 0); }

  void addHandler(PHandler pfunc) final { m_handlers.emplace_back(pfunc); }

  void runReadLoop() final {
    IPCHeader packet = {};

    while (true) {
      size_t   bufsz = sizeof(packet);
      uint32_t prio  = 0;

      mq.receive(&packet, bufsz, bufsz, prio);
      if (bufsz != sizeof(packet)) throw std::exception("Failed to receive packet");
      char* data = packet.bodySize > 0 ? new char[packet.bodySize] : nullptr;
      if (packet.bodySize > 0) {
        prio = 0, bufsz = 0;
        mq.receive(data, packet.bodySize, bufsz, prio);
      }
      for (auto handler: m_handlers)
        handler(packet.packetId, packet.bodySize, data);
      if (data) delete[] data;
    }
  }
};

std::unique_ptr<ICommunication> createIPC(const char* name) {
  return std::make_unique<Communication>(name);
}
