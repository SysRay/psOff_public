#include "utility/utility.h"

#include <array>
#include <stdint.h>

constexpr int _JBLEN_AMD = 20; /* Size of the jmp_buf on AMD64. */

struct unwinding_jmp_buf {
  uint64_t _jb[_JBLEN_AMD];
};

extern "C" {
SYSV_ABI bool unwinding_setjmp(unwinding_jmp_buf* data);

SYSV_ABI void unwinding_longjmp(unwinding_jmp_buf* data);
}