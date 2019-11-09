#include "debug.h"

#include <stdlib.h>
#include <stdarg.h>

#include "../system/File.h"

using namespace L;

static FILE* log_file = nullptr;

void L::init_log_file() {
  File("logs").make();
  char filepath[128] {};
  const Date date = Date::now();
  sprintf(filepath, "logs/%04d_%02d_%02d_%02d_%02d_%02d.log",
    date.year(), date.month(), date.day(), date.hour(), date.minute(), date.second());
  log_file = fopen(filepath, "w");
}
void L::error(const char* msg, ...) {
  va_list args;

  va_start(args, msg);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\nCallstack:\n");
  dump_stack(stderr);
  va_end(args);

  if(log_file) {
    va_start(args, msg);
    fprintf(log_file, "Error: ");
    vfprintf(log_file, msg, args);
    fprintf(log_file, "\nCallstack:\n");
    dump_stack(log_file);
    va_end(args);
    fflush(log_file);
  }

  debugbreak();
  exit(0xA55E2737);
}
void L::warning(const char* msg, ...) {
  va_list args;

  va_start(args, msg);
  fprintf(stderr, "Warning: ");
  vfprintf(stderr, msg, args);
  putc('\n', stderr);
  va_end(args);

  if(log_file) {
    va_start(args, msg);
    fprintf(log_file, "Warning: ");
    vfprintf(log_file, msg, args);
    putc('\n', log_file);
    va_end(args);
    fflush(log_file);
  }
}

void L::log(const char* msg, ...) {
  va_list args;

  va_start(args, msg);
  vfprintf(stdout, msg, args);
  putc('\n', stdout);
  va_end(args);

  if(log_file) {
    va_start(args, msg);
    vfprintf(log_file, msg, args);
    putc('\n', log_file);
    va_end(args);
  }
}
