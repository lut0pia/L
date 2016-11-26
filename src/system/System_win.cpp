#include "System.h"

#include <Windows.h>

using namespace L;
using namespace System;

String System::callGet(const char* cmd) {
  String wtr = "";
  FILE* pipe = _popen(cmd,"r");
  if(!pipe) L_ERROR("Couldn't open pipe in System");
  else {
    char buffer[128];
    while(!feof(pipe)) {
      if(fgets(buffer,128,pipe) != nullptr)
        wtr += buffer;
    }
    _pclose(pipe);
  }
  return wtr;
}
void System::sleep(const Time& t) {
  Sleep(t.milliseconds());
}
void System::beep(uint32_t frequency,uint32_t milliseconds) {
  ::Beep(frequency,milliseconds);
}
uint64_t System::ticks() {
  return __rdtsc();
}
void System::toClipboard(const String& data) {
  if(OpenClipboard(nullptr) && EmptyClipboard()) {
    HGLOBAL tmp = GlobalAlloc(0,data.size()+1); // Allocate global memory
    GlobalLock(tmp); // Lock it
    strcpy((char*)tmp,data); // Copy data
    GlobalUnlock(tmp); // Unlock it
    SetClipboardData(CF_TEXT,tmp);
    CloseClipboard();
  } else L_ERROR("Couldn't open clipboard.");
}
String System::fromClipboard() {
  HGLOBAL tmp;
  if(OpenClipboard(nullptr) && (tmp = GetClipboardData(CF_TEXT))) {
    String wtr((char*)tmp);
    CloseClipboard();
    return wtr;
  } else L_ERROR("Cannot get clipboard data.");
}
String System::pwd() {
  String wtr(callGet("cd"));
  wtr[wtr.size()-1] = slash; // Because there's a \n at the end
  return wtr;
}
Vector2i System::screenSize() {
  return Vector2i(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
}
String System::formatPath(String path) {
  path.replaceAll("/","\\");
  if(path != "\\" && (path.size()<2 || path[1]!=':'))
    path = System::pwd()+path;
  return path;
}
void* System::alloc(size_t size) {
  return VirtualAlloc(0,size,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
}
void System::dealloc(void* p) {
  VirtualFree(p,0,MEM_RELEASE);
}
