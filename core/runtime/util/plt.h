#pragma once

#include <iterator>
#include <stdint.h>

namespace {
struct JmpWithIndex {
  void setHandler(void* handler) {
    auto handlerAddr = (int64_t)(handler);
    auto ripAddr     = (int64_t)(&m_code[10]);
    auto offset64    = handlerAddr - ripAddr;
    auto offset32    = (uint32_t)offset64;

    *(uint32_t*)(&m_code[6]) = offset32;
  }

  void setIndex(uint32_t index) { *(uint32_t*)(&m_code[1]) = index; }

  static uint64_t getSize() { return 16; }

  private:
  // 68 00 00 00 00          push     <index>
  // E9 E0 FF FF FF          jmp      <handler>
  uint8_t m_code[16] = {0x68, 0x00, 0x00, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
};

} // namespace

struct CallPlt {
  explicit CallPlt(uint32_t tableSize) {
    for (uint32_t index = 0; index < tableSize; ++index) {
      auto c = (JmpWithIndex*)(getAddr(index));
      c->setIndex(index);
      c->setHandler(this);
    }
  }

  uint64_t getAddr(uint32_t index) { return (uint64_t)(&code[32] + JmpWithIndex::getSize() * index); }

  void setPltGot(uint64_t vaddr) { *(uint64_t*)(&code[2]) = vaddr; }

  static uint64_t getSize(uint32_t tableSize) { return 32 + JmpWithIndex::getSize() * tableSize; }

  private:
  // 0:  49 bb 88 77 66 55 44    movabs r11,0x1122334455667788
  // 7:  33 22 11
  // a:  41 ff 73 08             push   QWORD PTR [r11+0x8]
  // e:  41 ff 63 10             jmp    QWORD PTR [r11+0x10]
  uint8_t code[32] = {0x49, 0xBB, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x41, 0xFF,
                      0x73, 0x08, 0x41, 0xFF, 0x63, 0x10, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
};