#include "Stream.h"

using namespace L;

const char* Stream::line() {
  static char buffer[4096];
  char* w(buffer);
  nospace();
  while((*w = get())!='\n' && *w!='\r' && !end()) w++; // End of line or file
  *w = '\0'; // Null-end string
  return buffer;
}
const char* Stream::word() {
  static char buffer[1024];
  char* w(buffer);
  nospace();
  while(!isspace(*w = get()) && !end()) w++; // End of line or file
  *w = '\0'; // Null-end string
  return buffer;
}
const char* Stream::bufferize(size_t* size) {
  static char buffer[4096];
  char* w(buffer);
  while(!end() && w<buffer+4096) {*w++ = get();} // End of line or file
  if(w==buffer) return 0;
  else {
    *size = w-buffer;
    return buffer;
  }
}
