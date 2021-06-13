#include "System.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace L;
using namespace System;

void System::sleep(const Time& t) {
  usleep(t.microseconds());
}
String System::pwd() {
  String wtr;
  call("pwd", wtr);
  wtr[wtr.size()-1] = '/'; // Because there's a \n at the end
  return wtr;
}
Vector2i System::screen_size() {
  String tmp;
  call("xdpyinfo | grep 'dimensions:' | grep -o '[[:digit:]]\\+'", tmp);
  Array<String> res(tmp.explode('\n'));
  if(res.size() >= 2) {
    return Vector2i(ston<10, int>(res[0]), ston<10, int>(res[1]));
  }
  error("Couldn't determine screen size");
  return Vector2i(0, 0);
}

String System::formatPath(String path) {
  if(!path.size() || path[0u]!='/')
    path = System::pwd()+path;
  return path;
}

void* System::alloc(size_t size) {
  return mmap(0,size,PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
}
void System::dealloc(void*) {
  error("System::dealloc not implemented.");
}
