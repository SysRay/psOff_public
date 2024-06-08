#include "packets/EmulatorOpenGame.h"
#include "pipeprocess.h"

#include <format>
#include <stdio.h>
#include <string>
#include <thread>

int main() {
  PipeProcess* emuproc = CreatePipedProcess(".\\psoff.exe", "emulator");

  EmulatorOpenGamePacket packet("D:/ps4/games/Sonic Mania/eboot.bin", "", "D:/ps4/games/Sonic Mania patch/");
  packet.putPacketTo(emuproc);

  while (true)
    Sleep(5);

  return 0;
}
