#include "String.h"

using namespace L;
using namespace Pending;

String::String(const char* str) {
  do push(*str);
  while(*str++!='\0');
}
