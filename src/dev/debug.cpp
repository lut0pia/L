#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

using namespace L;

void L::error(const char* msg, ...) {
  va_list(args);
  fprintf(stderr, "Error: ");
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\nCallstack:\n");
  dump_stack();
  debugbreak();
  exit(0xA55E2737);
}
void L::warning(const char* msg, ...) {
  va_list(args);
  fprintf(stderr, "Warning: ");
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  putc('\n', stderr);
}
