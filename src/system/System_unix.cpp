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

String System::formatPath(String path) {
  if(!path.size() || path[0u]!='/')
    path = System::pwd()+path;
  return path;
}
