#pragma once

#include <cstring>
#include <new>
#include "math/math.h"

namespace L {
  template <class T, bool pod = false>
  inline void copy(T* dst, const T* src, size_t count) {
    while(count--)
      new(dst++) T(*src++);
  }
  template <class T>
  void swap(T& a, T& b) {
    char tmp[sizeof(T)];
    memcpy(tmp,&a,sizeof(T));
    memcpy(&a,&b,sizeof(T));
    memcpy(&b,tmp,sizeof(T));
  }

  template <class T>
  inline void swap(T* a, T* b, size_t count) {swap<char>((char*)a,(char*)b,count*sizeof(T));}
  template<>
  inline void swap(char* a, char* b, size_t count) {
    char buffer[16384];
    if(a==b) return;
    while(count>0) {
      size_t bufsize(min(min(count,(size_t)abs((intptr_t)a-(intptr_t)b)),(size_t)16384));
      memcpy(buffer,a,bufsize);
      memcpy(a,b,bufsize);
      memcpy(b,buffer,bufsize);
      a = a+bufsize;
      b = b+bufsize;
      count -= bufsize;
    }
  }
}
