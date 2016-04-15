#include "String.h"

#include <cctype>
#include "types.h"
#include "macros.h"

using namespace L;

String::String(const char* str) {
  do Array<char>::push(*str);
  while(*str++!='\0');
}
String::String(const char* buffer, size_t length) : Array<char>(length+1) {
  memcpy(&operator[](0),buffer,length);
  operator[](length) = '\0';
}
String::String(const String& str, size_t pos, size_t length)
  : Array<char>((length = (length==-1)?str.size()-pos:length)+1) {
  memcpy(&operator[](0),&str[pos],length);
  operator[](length) = '\0';
}
String& String::operator+=(const String& other) {
  Array<char>::pop();
  Array<char>::operator+=(other);
  return *this;
}

int String::findFirst(const String& str) const {
  for(uint32_t i(0); i<size(); i++)
    if(strncmp(&operator[](i),str,str.size())==0)
      return i;
  return -1;
}

Array<String> String::explode(char c, size_t limit) const {
  Array<String> wtr; // Will contain parts
  Array<size_t> delimiters(1,0); // Will contain delimiters for the split (already contains 0)
  size_t size(size());
  if(!size) return wtr;
  for(size_t i(0); (!limit || delimiters.size()<limit) && i<size; i++)
    if(operator[](i)==c) // Add delimiter
      delimiters.push(i+1);
  delimiters.push(size+1); // Add end delimiter
  for(size_t i(0); i<delimiters.size()-1; i++)
    wtr.push(substr(delimiters[i],delimiters[i+1]-delimiters[i]-1));
  return wtr;
}
size_t String::endOf(size_t i, bool dquotesEscape) const {
  char startChar = operator[](i), endChar('\0');
  size_t level(0), move(1);
  bool inQuotes(false);
  switch(startChar) {
    case '{':
      endChar = '}';
      break;
    case '}':
      endChar = '{';
      move = -1;
      break;
    case '(':
      endChar = ')';
      break;
    case ')':
      endChar = '(';
      move = -1;
      break;
    case '[':
      endChar = ']';
      break;
    case ']':
      endChar = '[';
      move = -1;
      break;
    default:
      L_ERROR("Unhandled startChar in EndOf");
      break;
  }
  while((i+=move) < size() && i!=(size_t)~0) {
    if(!inQuotes) {
      if(dquotesEscape && (*this)[i]=='"' && (!i || (*this)[i-1]!='\\'))
        inQuotes = true;
      else if((*this)[i]==startChar)
        level++;
      else if((*this)[i]==endChar) {
        if(level)
          level--;
        else
          return i;
      }
    } else if((*this)[i]=='"' && (!i || (*this)[i-1]!='\\'))
      inQuotes = false;
  }
  return 0;
}

String& String::replaceAll(const String& search, const String& replace) {
  for(uint32_t i(0); i<size(); i++)
    if(strncmp(&operator[](i),search,search.size())==0) {
      this->replace(i,search.size(),replace);
      i += replace.size()-1;
    }
  return *this;
}
String& String::trimLeft(size_t n) {
  erase(0,n);
  return *this;
}
String& String::trimRight(size_t n) {
  erase(size()-n,n);
  return *this;
}
String& String::trim(char c) {
  while(size()) {
    if(operator[](0)==c) trimLeft();
    else if(operator[](size()-1)==c) trimRight();
    else break;
  }
  return *this;
}
String& String::trim(const char* s) {
  while(size()) {
    if(strchr(s,operator[](0))) trimLeft();
    else if(strchr(s,operator[](size()-1))) trimRight();
    else break;
  }
  return *this;
}
String& String::padLeft(size_t n,char c) {
  if(size()<n) this->insert(0,String(n-size(),c));
  return *this;
}
String& String::toLower() {
  char* c(*this);
  while(*c!='\0') {
    *c = tolower(*c);
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
      *c = (first)?toupper(*c):tolower(*c);
      first = false;
    }
    c++;
  }
  return *this;
}
