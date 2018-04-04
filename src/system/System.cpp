#include "System.h"

using namespace L;
using namespace System;

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
