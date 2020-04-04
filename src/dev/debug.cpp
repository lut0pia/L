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
  verror(msg, args);
}
void L::warning(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  vwarning(msg, args);
  va_end(args);
}

void L::log(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  vlog(msg, args);
  va_end(args);
}

void L::verror(const char* msg, va_list args) {
  fprintf(stderr, "Error: ");
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\nCallstack:\n");
  dump_stack(stderr);

  if(log_file) {
    fprintf(log_file, "Error: ");
    vfprintf(log_file, msg, args);
    fprintf(log_file, "\nCallstack:\n");
    dump_stack(log_file);
    fflush(log_file);
  }

  debugbreak();
  exit(0xA55E2737);
}
void L::vwarning(const char* msg, va_list args) {
  fprintf(stderr, "Warning: ");
  vfprintf(stderr, msg, args);
  putc('\n', stderr);

  if(log_file) {
    fprintf(log_file, "Warning: ");
    vfprintf(log_file, msg, args);
    putc('\n', log_file);
    fflush(log_file);
  }
}
void L::vlog(const char* msg, va_list args) {
  vfprintf(stdout, msg, args);
  putc('\n', stdout);

  if(log_file) {
    vfprintf(log_file, msg, args);
    putc('\n', log_file);
  }
}
