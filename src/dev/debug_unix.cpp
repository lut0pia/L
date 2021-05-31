#include "debug.h"

#include <signal.h>

using namespace L;

void L::dump_stack(FILE*) {

}

void L::debugbreak() {
  raise(SIGTRAP);
}

bool L::set_data_breakpoint(const void* ptr, size_t size, L::DataBreakpointType type) {
  return false;
}
bool L::unset_data_breakpoint(const void* ptr) {
  return false;
}
