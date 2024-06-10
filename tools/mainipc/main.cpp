#include "packets/EmulatorExecRequest.h"
#include "packets/EmulatorLoadGame.h"
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
        IPCEmulatorExecRequest eer(pproc, phead);

        auto self = emuproc->reader;

        emuproc         = CreatePipedProcess(".\\psoff.exe", "", "emulator");
        emuproc->reader = self;

        IPCEmulatorLoadGame packet(eer.getExecutable(), gameroot, updateroot);
        packet.addArguments(eer.getArguments());
        packet.putPacketTo(emuproc);

        IPCEmulatorRunGame grun_packet;
        grun_packet.putPacketTo(emuproc);
      } break;

      default: break;
    }
  };

  IPCEmulatorLoadGame gload_packet(gamexec, gameroot, updateroot);
  gload_packet.addArguments(nullptr);
  gload_packet.putPacketTo(emuproc);

  IPCEmulatorRunGame grun_packet;
  grun_packet.putPacketTo(emuproc);

  while (true)
    Sleep(5);

  return 0;
}
