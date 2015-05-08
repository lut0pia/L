#include "System.h"

using namespace L;
using namespace System;

#if defined L_UNIX
#include <unistd.h>
#include <curses.h>
#endif

#include "Directory.h"
#include "../Exception.h"
#include "../stl.h"

String System::callGet(const String& cmd) {
  String wtr = "";
  FILE* pipe = popen(cmd.c_str(),"r");
  if(!pipe) throw Exception("Couldn't open pipe in System");
  else {
    char buffer[128];
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
        wtr += buffer;
    }
    pclose(pipe);
  }
  return wtr;
}
int System::call(const String& cmd) {
  return system(cmd.c_str());
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
  throw Exception("Cannot beep under UNIX.");
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
  if(OpenClipboard(NULL) && EmptyClipboard()) {
    HGLOBAL tmp = GlobalAlloc(0,data.size()+1); // Allocate global memory
    GlobalLock(tmp); // Lock it
    strcpy((char*)tmp,data.c_str()); // Copy data
    GlobalUnlock(tmp); // Unlock it
    SetClipboardData(CF_TEXT,tmp);
    CloseClipboard();
  } else throw Exception("Couldn't open clipboard.");
#endif
}
String System::fromClipboard() {
#if defined L_WINDOWS
  HGLOBAL tmp;
  if(OpenClipboard(NULL) && (tmp = GetClipboardData(CF_TEXT))) {
    String wtr((char*)tmp);
    CloseClipboard();
    return wtr;
  }
#endif
  throw Exception("Cannot get clipboard data.");
}
String System::gEnv(const String& name) {
  return getenv(name.c_str());
}
Point2i System::screenSize() {
#if defined L_WINDOWS
  return Point2i(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
#endif
}
void System::sConsoleCursorPosition(ushort x,ushort y) {
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
  throw Exception("Closing the console under UNIX is impossible.");
#endif
}

String System::formatPath(String path) {
#if defined L_WINDOWS
  path = path.replaceAll("/","\\");
#endif
#if defined L_WINDOWS
  if(path != "\\" && (path.size()<2 || path[1]!=':'))
#elif defined L_UNIX
  if(!path.size() || path[0]!='/')
#endif
    path = Directory::current.gPath()+path;
  return path;
}
String System::pathDirectory(String path) {
  return path.substr(0,1+path.find_last_of(slash));
}
String System::pathFile(String path) {
  return path.substr(1+path.find_last_of(slash));
}

