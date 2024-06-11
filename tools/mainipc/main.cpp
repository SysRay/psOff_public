#include "packets/EmulatorAddArguments.h"
#include "packets/EmulatorExecRequest.h"
#include "packets/EmulatorLoadGame.h"
#include "packets/EmulatorRunGame.h"
#include "pipeprocess.h"

#include <chrono>

int main() {
  PipeProcess* emuproc = CreatePipedProcess(".\\psoff.exe", "--d", "emulator");

  std::string gamexec    = "D:/ps4/games/Call of Duty Ghosts/eboot.bin";
  std::string gameroot   = "";
  std::string updateroot = "";

  emuproc->reader = [&](PipeProcess* pproc, IPCHeader* phead) {
    switch ((IpcEvent)phead->packetId) {
      case IpcEvent::EMU_EXEC_REQUEST: {
        IPCEmulatorExecRequest eer(pproc, phead);

        auto self = emuproc->reader;

        std::string pipeName = std::format("emulator-rerun-{}", std::chrono::system_clock::now().time_since_epoch().count());

        emuproc         = CreatePipedProcess(".\\psoff.exe", "", pipeName.c_str());
        emuproc->reader = self;

        IPCEmulatorLoadGame load_packet(eer.getExecutable(), gameroot, updateroot);
        load_packet.putPacketTo(emuproc);

        IPCEmulatorAddArguments args_packet(eer.getArguments());
        args_packet.putPacketTo(emuproc);

        IPCEmulatorRunGame grun_packet;
        grun_packet.putPacketTo(emuproc);
      } break;

      default: break;
    }
  };

  {
    IPCEmulatorLoadGame gload_packet(gamexec, gameroot, updateroot);
    gload_packet.putPacketTo(emuproc);
  }

  {
    IPCEmulatorRunGame grun_packet;
    grun_packet.putPacketTo(emuproc);
  }

  while (true)
    Sleep(5);

  return 0;
}
