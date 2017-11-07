#include "System.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace L;
using namespace System;

String System::callGet(const char* cmd) {
  String wtr = "";
  FILE* pipe = popen(cmd,"r");
  if(!pipe) error("Couldn't open pipe in System");
  else {
    char buffer[128];
    while(!feof(pipe)) {
      if(fgets(buffer,128,pipe) != nullptr)
        wtr += buffer;
    }
    pclose(pipe);
  }
  return wtr;
}
void System::sleep(const Time& t) {
  usleep(t.microseconds());
}
void System::beep(uint32_t frequency,uint32_t milliseconds) {
  error("Cannot beep under UNIX.");
}
void System::toClipboard(const String& data) {
  error("System::toClipboard not implemented.");
}
String System::fromClipboard() {
  error("System::fromClipboard not implemented.");
}
String System::pwd() {
  String wtr(callGet("pwd"));
  wtr[wtr.size()-1] = '/'; // Because there's a \n at the end
  return wtr;
}
Vector2i System::screenSize() {
  Array<String> res(callGet("xdpyinfo | grep 'dimensions:' | grep -o '[[:digit:]]\\+'").explode('\n'));
  if(res.size()>=2)
    return Vector2i(ston<10,int>(res[0]),ston<10,int>(res[1]));
}

String System::formatPath(String path) {
  if(!path.size() || path[0]!='/')
    path = System::pwd()+path;
  return path;
}

void* System::alloc(size_t size) {
  return mmap(0,size,PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
}
void System::dealloc(void* p) {
  error("System::dealloc not implemented.");
}
