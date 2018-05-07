#include "System.h"

#include <stdio.h>

using namespace L;
using namespace System;

#ifdef L_WINDOWS
#define popen _popen
#define pclose _pclose
#endif

int System::call(const char* cmd, String& output) {
  output.clear();
  FILE* pipe(popen(cmd, "r"));
  if(pipe) {
    char buffer[512];
    while(!feof(pipe)) {
      if(size_t count = fread(buffer, 1, sizeof(buffer), pipe)) {
        size_t old_size(output.size());
        output.size(old_size+count);
        memcpy(output+old_size, buffer, count);
      }
    }
    return pclose(pipe);
  } else error("Couldn't open pipe in System");
  return -1;
}
int System::call(const char* cmd) {
  return system(cmd);
}
void System::sleep(int milliseconds) {
  sleep(Time(0,milliseconds));
}
String System::pathDirectory(const String& path) {
  return path.substr(0,1+path.find_last(slash));
}
String System::pathFile(const String& path) {
  return path.substr(1+path.find_last(slash));
}
