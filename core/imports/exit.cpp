#define __APICALL_EXTERN
#include "imports_exit.h"
#undef __APICALL_EXTERN

#include "core/dmem/dmem.h"

/**
 * @brief Exit handling of the core lib
 *
 */

void core_exit() {

  // Cleanup fixed memory (host&gpu)
  accessPysicalMemory().deinit();
}