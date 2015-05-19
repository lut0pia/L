#include "String.h"

#include "../Exception.h"
#include "../macros.h"
#include "Set.h"
#include "../containers/Array.h"

using namespace L;

const byte String::quotations = 1;
const byte String::parentheses = 2;
const byte String::curlybrackets = 4;
const byte String::squarebrackets = 8;
const byte String::allbrackets = String::quotations | String::parentheses | String::curlybrackets | String::squarebrackets;

String::String() : std::string() {}
String::String(const std::string& str) : std::string(str) {}
String::String(const std::string& str, size_t pos, size_t len) : std::string(str,pos,len) {}
String::String(const char* s) : std::string(s) {}
String::String(const char* s, size_t n) : std::string(s,n) {}
String::String(size_t n, char c) : std::string(n,c) {}

String String::substr(size_t pos, size_t len) const {
  return String(*this,pos,len);
}
List<String> String::escapedExplode(char c, byte escapeBrackets, uint limit) const {
  List<String> wtr; // Will contain parts
  Array<uint> delimiters(1,0); // Will contain delimiters for the split (already contains 0)
  List<byte> brackets(1,0);
  size_t size(this->size());
  if(!size) return wtr;
  for(uint i(0); (!limit || delimiters.size()<limit) && i<size; i++) {
    if((*this)[i] == '"' && (escapeBrackets & quotations)) { // There's a quotation and it's an escape
      if(brackets.back()==quotations) // Closing
        brackets.pop_back();
      else                              // Opening
        brackets.push_back(quotations);
    } else if(brackets.back()!=quotations) { // We're not in quotes ((), {}, [] are effective)
      if((*this)[i] == '(' && (escapeBrackets & parentheses)) // There's a parenthese opening and it's an escape
        brackets.push_back(parentheses);
      else if((*this)[i] == ')' && brackets.back()==parentheses) // There's a parenthese closing and we're in parentheses
        brackets.pop_back();
      else if((*this)[i] == '{' && (escapeBrackets & curlybrackets)) // There's a curly bracket opening and it's an escape
        brackets.push_back(curlybrackets);
      else if((*this)[i] == '}' && brackets.back()==curlybrackets) // There's a curly bracket closing and we're in curly brackets
        brackets.pop_back();
      else if((*this)[i] == '[' && (escapeBrackets & squarebrackets)) // There's a square bracket opening and it's an escape
        brackets.push_back(squarebrackets);
      else if((*this)[i] == ']' && brackets.back()==squarebrackets) // There's a square bracket closing and we're in a square bracket
        brackets.pop_back();
    }
    if((*this)[i]==c && !brackets.back()) { // Add delimiter
      delimiters.push(i+1);
    }
  }
  delimiters.push(size+1);
  for(uint i(0); i<delimiters.size()-1; i++)
    wtr.push_back(substr(delimiters[i],delimiters[i+1]-delimiters[i]-1));
  return wtr;
}
List<String> String::explode(char c, size_t limit) const {
  List<String> wtr; // Will contain parts
  Array<uint> delimiters(1,0); // Will contain delimiters for the split (already contains 0)
  size_t size(this->size());
  if(!size) return wtr;
  for(size_t i(0); (!limit || delimiters.size()<limit) && i<size; i++)
    if((*this)[i]==c) // Add delimiter
      delimiters.push(i+1);
  delimiters.push(size+1); // Add end delimiter
  for(uint i(0); i<delimiters.size()-1; i++)
    wtr.push_back(substr(delimiters[i],delimiters[i+1]-delimiters[i]-1));
  return wtr;
}
String String::replaceAll(const String& search, const String& replace) const {
  String wtr(*this);
  uint pos(-replace.size());
  while((pos=wtr.find(search,pos+replace.size()))!=(uint)-1)
    wtr.replace(pos,search.size(),replace);
  return wtr;
}
void String::trimLeft(size_t n) {
  erase(0,n);
}
void String::trimRight(size_t n) {
  erase(size()-n,n);
}
String String::trim(char c) const {
  String wtr(*this);
  while(wtr.size()) {
    if(wtr[0]==c)wtr.trimLeft();
    else if(wtr[wtr.size()-1]==c)wtr.trimRight();
    else break;
  }
  return wtr;
}
String String::trim(const String& c) const {
  String wtr(*this);
  while(wtr.size()) {
    if(c.find(wtr[0])!=npos)wtr.trimLeft();
    else if(c.find(wtr[wtr.size()-1])!=npos)wtr.trimRight();
    else break;
  }
  return wtr;
}
String String::padLeft(size_t n, char c) const {
  String wtr(*this);
  return (wtr.size()<n) ? String(n-wtr.size(),c)+wtr : wtr;
}
String String::toLower() const {
  String wtr(*this);
  for(size_t i=0; i<wtr.size(); i++)
    wtr[i] = tolower(wtr[i]);
  return wtr;
}
String String::capitalize() const {
  String wtr(*this);
  bool first = true;
  for(size_t i=0; i<wtr.size(); i++) {
    switch(wtr[i]) {
      case ' ':
      case '.':
      case '-':
      case '/':
      case '(':
        first = true;
        break;
      default:
        if(first) {
          wtr[i] = toupper(wtr[i]);
          first = false;
        }
        break;
    }
  }
  return wtr;
}
size_t String::endOf(size_t i, bool dquotesEscape) const {
  char startChar = (*this)[i], endChar('\0');
  size_t level(0), move(-1);
  bool inQuotes = false;
  switch(startChar) {
    case '{':
      endChar = '}';
      move = 1;
      break;
    case '}':
      endChar = '{';
      move = -1;
      break;
    case '(':
      endChar = ')';
      move = 1;
      break;
    case ')':
      endChar = '(';
      move = -1;
      break;
    case '[':
      endChar = ']';
      move = 1;
      break;
    case ']':
      endChar = '[';
      move = -1;
      break;
    default:
      throw Exception("Unhandled startChar in EndOf");
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

String L::operator+(const String& lhs, const String& rhs) {
  String wtr(lhs);
  wtr.append(rhs);
  return wtr;
}
String L::operator+(const String& lhs, const char* rhs) {
  String wtr(lhs);
  wtr.append(rhs);
  return wtr;
}
String L::operator+(const char* lhs, const String& rhs) {
  String wtr(lhs);
  wtr.append(rhs);
  return wtr;
}
String L::operator+(const String& lhs, char rhs) {
  String wtr(lhs);
  wtr.push_back(rhs);
  return wtr;
}
String L::operator+(char lhs, const String& rhs) {
  String wtr(rhs);
  wtr.insert(0,1,lhs);
  return wtr;
}
