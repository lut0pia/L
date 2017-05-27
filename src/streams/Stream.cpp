#include "Stream.h"

#include "../macros.h"

using namespace L;

const char* Stream::line() {
  static char buffer[4096];
  line(buffer, sizeof(buffer));
  return buffer;
}
void Stream::line(char* buffer, size_t size){
  char* w(buffer);
  char* buffer_end(buffer+size);
  nospace();
  while((*w = get())!='\n' && *w!='\r' && !end()) // Not end of line or file
    if(++w == buffer_end)
      L_ERROR("Buffer was too small");
  *w = '\0'; // Null-end string
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
