#include "String.h"

using namespace L;
using namespace Pending;

Pending::String::String(const char* str) {
  do push(*str);
  while(*str++!='\0');
}
Pending::String::String(const String& str, size_t pos, size_t length) {
  if(length==-1)
    length = str.size()-pos;
  capacity(length+1);
  while(length--)
    push(str[pos++]);
  push('\0');
}
Pending::String Pending::String::substr(size_t pos, size_t length) const {
  return String(*this,pos,length);
}
