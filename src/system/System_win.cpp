#include "System.h"

#include <Windows.h>

using namespace L;
using namespace System;

void System::sleep(const Time& t) {
  Sleep(t.milliseconds());
}
void System::beep(uint32_t frequency,uint32_t milliseconds) {
  ::Beep(frequency,milliseconds);
}
void System::toClipboard(const String& data) {
  if(OpenClipboard(nullptr) && EmptyClipboard()) {
    HGLOBAL tmp = GlobalAlloc(0,data.size()+1); // Allocate global memory
    GlobalLock(tmp); // Lock it
    strcpy((char*)tmp,data); // Copy data
    GlobalUnlock(tmp); // Unlock it
    SetClipboardData(CF_TEXT,tmp);
    CloseClipboard();
  } else error("Couldn't open clipboard.");
}
String System::fromClipboard() {
  HGLOBAL tmp;
  if(OpenClipboard(nullptr) && (tmp = GetClipboardData(CF_TEXT))) {
    String wtr((char*)tmp);
    CloseClipboard();
    return wtr;
  } else error("Cannot get clipboard data.");
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
