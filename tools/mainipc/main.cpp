#include "packets/EmulatorOpenGame.h"
#include "pipeprocess.h"

int main() {
  PipeProcess* emuproc = CreatePipedProcess(".\\psoff.exe", "", "emulator");

  emuproc->reader = [](PipeProcess* pproc, PacketHeader* phead) {};

  EmulatorOpenGamePacket packet("D:/ps4/games/Sonic Mania/eboot.bin", "", "D:/ps4/games/Sonic Mania patch/");
  packet.putPacketTo(emuproc);

  while (true)
    Sleep(5);

  return 0;
}
