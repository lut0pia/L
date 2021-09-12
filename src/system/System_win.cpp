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
String System::formatPath(String path) {
  // TODO: All paths should use / (at least in frontend) for consistency
  if(path != "\\" && (path.size()<2 || path[1]!=':'))
    path = System::pwd()+path;
  path.replace_all("\\", "/");
  return path;
}
