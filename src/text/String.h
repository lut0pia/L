#pragma once

#include <cstring>
#include "../container/Array.h"
#include "../stream/serial.h"

namespace L {
  class String: private Array<char> {
    public:
      inline String() : Array<char>(1,'\0') {}
      inline String(size_t s, char c) : Array<char>(s,c) {Array<char>::push('\0');}
      String(const char*);
      String(const char*,size_t);
      String(const String&, size_t start, size_t length = -1);

      inline String operator+(const String& other) const {
        String wtr(*this);
        return wtr += other;
      }
      String& operator+=(const String& other);
      inline bool operator==(const char* other) const {return !strcmp(*this,other);}
      inline bool operator!=(const char* other) const {return !operator==(other);}
      inline bool operator<(const char* other) const {return strcmp(*this,other)<0;}
      inline bool operator>(const char* other) const {return strcmp(*this,other)>0;}

      inline bool operator==(const String& other) const {return size()==other.size() && !strcmp(*this,other);}
      inline bool operator!=(const String& other) const {return !operator==(other);}
      inline bool operator<(const String& other) const {return strcmp(*this,other)<0;}
      inline bool operator>(const String& other) const {return strcmp(*this,other)>0;}

      // Search methods
      inline intptr_t findFirst(char c) const {return strchr(*this,c)-operator const char*();}
      int findFirst(const String&) const;
      inline intptr_t findLast(char c) const {return strrchr(*this,c)-operator const char*();}
      inline String substr(size_t pos, size_t length = -1) const {return String(*this,pos,length);}
      int count(char) const;

      Array<String> explode(char c, size_t limit = 0) const; // Splits and returns an array of each part (limit is the max number of parts)

      // Self modifiers
      String& replaceAll(const String& search, const String& replace); // Replaces search by replace
      String& trimLeft(size_t=1); // Remove size_t left chars
      String& trimRight(size_t=1); // Remove size_t right chars
      String& trim(char = ' '); // Remove all chars left and right that are char
      String& trim(const char*); // Remove all chars left and right that are in const char*
      String& padLeft(size_t,char); // Add char to left until size==size_t
      String& toLower(); // To lowercase
      String& capitalize(); // Capitalize first letter of all words

      // Array redirections
      inline void pop() {erase(size()-1);}
      inline void push(char c) {insert(size(),c);}
      inline void insert(size_t i,char c) {Array<char>::insert(i,c);}
      inline void insert(size_t i,const String& str) {Array<char>::insertArray(i,str,str.size());}
      inline void replace(uintptr_t i, size_t len, const String& str) {Array<char>::replaceArray(i,len,str,str.size());}
      inline void erase(size_t i,size_t count=1) {Array<char>::erase(i,count);}
      inline void clear() {Array<char>::size(1); Array<char>::operator[](0)='\0';}
      inline void size(size_t s) {Array<char>::size(s+1); Array<char>::back()='\0';} // Add one because of '\0'
      inline size_t size() const {return Array<char>::size()-1;} // Subtract one because of '\0'
      inline bool empty() const {return Array<char>::size()<=1;} // Always has trailing '\0'
      inline operator const char*() const {return &Array<char>::operator[](0);}
      inline operator char*() {return &Array<char>::operator[](0);}
      inline const char& operator[](uintptr_t i) const {return Array<char>::operator[](i);}
      inline char& operator[](uintptr_t i) {return Array<char>::operator[](i);}
      using Array<char>::begin;
      inline char* end() { return &operator[](size()); }
      inline const char* end() const { return &operator[](size()); };
  };
  inline String operator+(const char* a, const String& b) {return String(a)+b;}
  inline Stream& operator<<(Stream &s,const String& v) { s.write(v.begin(),v.size()); return s; }
  inline Stream& operator<(Stream &s, const String& v) { s << v.size() << ' '; s.write(v.begin(), v.size()); return s << '\n'; }
  inline Stream& operator>(Stream &s, String& v) { size_t size; s > size; v.size(size); s.read(v.begin(), size); return s; }
  inline uint32_t hash(const String& v){ return hash(v.begin()); }
}
