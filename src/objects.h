#ifndef DEF_L_objects
#define DEF_L_objects

#include <cstring>
#include <new>
#include "math/math.h"

namespace L {
  template <class T, typename... Args>
  inline void construct(T& v, Args&&... args) {
    new(&v) T(args...);
  }
  template <class T, typename... Args>
  inline void construct(T* p, size_t count, Args&&... args) {
    while(count--)
      new(p++) T(args...);
  }
  template <class T>
  inline void destruct(T& v) {
    v.~T();
  }
  template <class T>
  inline void destruct(T* p, size_t count) {
    while(count--)
      (p++)->~T();
  }
  template <class T, typename... Args>
  inline void reconstruct(T& v, Args&&... args) {
    destruct(v);
    construct(v,args...);
  }
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

#endif

