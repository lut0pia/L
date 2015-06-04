#include "Stream.h"

using namespace L;

const char* Stream::line() {
  static char buffer[2048];
  char* w(buffer);
  nospace();
  while((*w++ = get())!='\n') {}
  unget('\n');
  *--w = '\0';
  return buffer;
}
