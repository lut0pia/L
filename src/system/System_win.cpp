#include "System.h"

#include <Windows.h>

using namespace L;
using namespace System;

void System::sleep(const Time& t) {
  Sleep(DWORD(t.milliseconds()));
}
String System::pwd() {
  String wtr;
  call("cd", wtr);
  wtr[wtr.size()-1] = slash; // Because there's a \n at the end
  return wtr;
}
Vector2i System::screenSize() {
  return Vector2i(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
}
String System::formatPath(String path) {
  // TODO: All paths should use / (at least in frontend) for consistency
  if(path != "\\" && (path.size()<2 || path[1]!=':'))
    path = System::pwd()+path;
  path.replaceAll("\\", "/");
  return path;
}
void* System::alloc(size_t size) {
  return VirtualAlloc(0,size,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
}
void System::dealloc(void* p) {
  VirtualFree(p,0,MEM_RELEASE);
}
