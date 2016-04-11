#include "System.h"

using namespace L;
using namespace System;

String System::callGet(const char* cmd) {
  String wtr = "";
  FILE* pipe = popen(cmd,"r");
  if(!pipe) L_ERROR("Couldn't open pipe in System");
  else {
    char buffer[128];
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != nullptr)
        wtr += buffer;
    }
    pclose(pipe);
  }
  return wtr;
}
int System::call(const char* cmd) {
  return system(cmd);
}
void System::sleep(int milliseconds) {
  sleep(Time(0,milliseconds));
}
void System::sleep(const Time& t) {
#if defined L_WINDOWS
  Sleep(t.milliseconds());
#elif defined L_UNIX
  usleep(t.microseconds());
#endif
}
void System::beep(uint frequency, uint milliseconds) {
#if defined L_WINDOWS
  ::Beep(frequency,milliseconds);
#elif defined L_UNIX
  L_ERROR("Cannot beep under UNIX.");
#endif
}
ullong System::ticks() {
#if defined _MSC_VER // MSVC
  return __rdtsc();
#else // GCC
  ullong wtr;
  __asm__ __volatile__
  (
    "cpuid \n"
    "rdtsc \n"
    "leal %0, %%ecx \n"
    "movl %%eax, (%%ecx) \n"
    "movl %%edx, 4(%%ecx)" :: "m"(wtr) : "eax", "ebx", "ecx", "edx"
  );
  return wtr;
#endif
}
void System::toClipboard(const String& data) {
#if defined L_WINDOWS
  if(OpenClipboard(nullptr) && EmptyClipboard()) {
    HGLOBAL tmp = GlobalAlloc(0,data.size()+1); // Allocate global memory
    GlobalLock(tmp); // Lock it
    strcpy((char*)tmp,data); // Copy data
    GlobalUnlock(tmp); // Unlock it
    SetClipboardData(CF_TEXT,tmp);
    CloseClipboard();
  } else L_ERROR("Couldn't open clipboard.");
#endif
}
String System::fromClipboard() {
#if defined L_WINDOWS
  HGLOBAL tmp;
  if(OpenClipboard(nullptr) && (tmp = GetClipboardData(CF_TEXT))) {
    String wtr((char*)tmp);
    CloseClipboard();
    return wtr;
  }
#endif
  L_ERROR("Cannot get clipboard data.");
}
String System::env(const char* name) {
  return getenv(name);
}
String System::pwd() {
  String wtr;
#if defined L_WINDOWS
  wtr = callGet("cd");
#elif defined L_UNIX
  wtr = callGet("pwd");
#endif
  wtr[wtr.size()-1] = slash; // Because there's a \n at the end
  return wtr;
}
Vector2i System::screenSize() {
#if defined L_WINDOWS
  return Vector2i(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
#endif
}
void System::consoleCursorPosition(ushort x,ushort y) {
#if defined L_WINDOWS
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD position = {(short)x,(short)y};
  SetConsoleCursorPosition(hStdout,position);
#elif defined L_UNIX
  move(y,x);
  refresh();
#endif
}
void System::clearConsole() {
#if defined L_WINDOWS
  call("cls");
#elif defined L_UNIX
  call("clear");
#endif
}
void System::closeConsole() {
#if defined L_WINDOWS
  FreeConsole();
#elif defined L_UNIX
  L_ERROR("Closing the console under UNIX is impossible.");
#endif
}

String System::formatPath(String path) {
#if defined L_WINDOWS
  path.replaceAll("/","\\");
#endif
#if defined L_WINDOWS
  if(path != "\\" && (path.size()<2 || path[1]!=':'))
#elif defined L_UNIX
  if(!path.size() || path[0]!='/')
#endif
    path = System::pwd()+path;
  return path;
}
String System::pathDirectory(const String& path) {
  return path.substr(0,1+path.findLast(slash));
}
String System::pathFile(const String& path) {
  return path.substr(1+path.findLast(slash));
}

void* System::alloc(size_t size) {
#if defined L_WINDOWS
  return VirtualAlloc(0,size,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
#elif defined L_UNIX
  return mmap(0,size,PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}
void System::dealloc(void* p) {
#if defined L_WINDOWS
  VirtualFree(p,0,MEM_RELEASE);
#elif defined L_UNIX
#endif
}
