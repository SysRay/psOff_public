#include "../ipcpacket.h"

class IPCEmulatorRunGame: public IPCPacket {
  public:
  IPCEmulatorRunGame(): IPCPacket(IpcEvent::EMU_RUN_GAME) {}
};
