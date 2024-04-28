#include "logging.h"

LOG_DEFINE_MODULE(MAIN);

int main() {

  __Log::flush();
  return 0;
}