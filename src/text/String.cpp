#include "String.h"

#include <cctype>
#include "../macros.h"

using namespace L;

String::String(const char* buffer, size_t length) : Array<char>(length+1) {
  memcpy(&operator[](0),buffer,length);
  operator[](length) = '\0';
}
String::String(const String& str, size_t pos, size_t length)
  : Array<char>((length = (length == size_t(-1)) ? str.size() - pos : length) + 1) {
  memcpy(&operator[](0),&str[pos],length);
  operator[](length) = '\0';
}
String& String::operator+=(const String& other) {
  Array<char>::pop();
  Array<char>::operator+=(other);
  return *this;
}

uintptr_t String::find_first(const String& str) const {
  for(uintptr_t i = 0; i < _size - 1; i++) {
    if(strncmp(_data + i, str, str._size - 1) == 0) {
      return i;
    }
  }
  return UINTPTR_MAX;
}
int String::count(char search) const {
  int wtr(0);
  for(char c : *this)
    if(c==search)
      wtr++;
  return wtr;
}

Array<String> String::explode(char c, size_t limit) const {
  Array<String> wtr; // Will contain parts
  Array<size_t> delimiters(1,0); // Will contain delimiters for the split (already contains 0)
  if(!size()) return wtr;
  for(size_t i(0); (!limit || delimiters.size()<limit) && i<size(); i++)
    if(operator[](i)==c) // Add delimiter
      delimiters.push(i+1);
  delimiters.push(size()+1); // Add end delimiter
  for(size_t i(0); i<delimiters.size()-1; i++)
    if(delimiters[i]<delimiters[i+1]-1)
      wtr.push(substr(delimiters[i],delimiters[i+1]-delimiters[i]-1));
  return wtr;
}

String& String::replace_all(const String& search, const String& replace) {
  for(uintptr_t i(0); i<size(); i++)
    if(strncmp(&operator[](i),search,search.size())==0) {
      this->replace(i,search.size(),replace);
      i += replace.size()-1;
    }
  return *this;
}
String& String::trim_left(size_t n) {
  erase(0,n);
  return *this;
}
String& String::trim_right(size_t n) {
  erase(size()-n,n);
  return *this;
}
String& String::trim(char c) {
  while(size()) {
    if(operator[](0)==c) trim_left();
    else if(operator[](size()-1)==c) trim_right();
    else break;
  }
  return *this;
}
String& String::trim(const char* s) {
  while(size()) {
    if(strchr(s,operator[](0))) trim_left();
    else if(strchr(s,operator[](size()-1))) trim_right();
    else break;
  }
  return *this;
}
String& String::pad_left(size_t n,char c) {
  if(size()<n) this->insert(0,String(n-size(),c));
  return *this;
}
String& String::to_lower() {
  char* c(*this);
  while(*c!='\0') {
    *c = char(tolower(*c));
    c++;
  }
  return *this;
}
String& String::capitalize()  {
  char* c(*this);
  bool first(true);
  while(*c!='\0') {
    if(Stream::isspace(*c))
      first = true;
    else {
      *c = char(first?toupper(*c):tolower(*c));
      first = false;
    }
    c++;
  }
  return *this;
}
