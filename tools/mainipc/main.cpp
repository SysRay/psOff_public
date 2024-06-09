#include "packets/EmulatorExecRequest.h"
#include "packets/EmulatorRunGame.h"
#include "pipeprocess.h"

int main() {
  PipeProcess* emuproc = CreatePipedProcess(".\\psoff.exe", "", "emulator");

  std::string gamexec    = "D:/ps4/games/Sonic Mania/eboot.bin";
  std::string gameroot   = "";
  std::string updateroot = "D:/ps4/games/Sonic Mania patch/";

  emuproc->reader = [&](PipeProcess* pproc, IPCHeader* phead) {
    switch ((IpcEvent)phead->packetId) {
      case IpcEvent::EMU_EXEC_REQUEST: {
        IPCEmulatorExecRequest eer;
        eer.readPacketFrom(pproc, phead);

        auto self = emuproc->reader;

        emuproc         = CreatePipedProcess(".\\psoff.exe", "", "emulator");
        emuproc->reader = self;

        IPCEmulatorRunGame packet(gamexec, gameroot, updateroot);
        packet.addArguments(nullptr);
        packet.putPacketTo(emuproc);
      } break;

      default: break;
    }
  };

  IPCEmulatorRunGame packet(gamexec, gameroot, updateroot);
  packet.addArguments(nullptr);
  packet.putPacketTo(emuproc);

  while (true)
    Sleep(5);

  return 0;
}
