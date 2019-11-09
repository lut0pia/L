#include "debug.h"

#include <signal.h>

using namespace L;

void L::dump_stack(FILE* stream) {

}

void L::debugbreak() {
  raise(SIGTRAP);
}
