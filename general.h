#ifndef DEF_L_general
#define DEF_L_general

#include <iostream>
#include <cstring>
#include "types.h"

namespace L {
  // Number functions :
  unsigned long factorial(unsigned long n);
  uint digit(uint d, uint b, uint n);
  template <class T>
  inline T log(T x, T base) {
    return std::log(x)/std::log(base);
  }
  template <class T, typename... Args>
  inline void reconstruct(T& v, Args&&... args) {
    v.~T();
    new(&v) T(args...);
  }

  // Template functions :
  template <class T>
  bool InBetween(const T& n, const T& a, const T& b) {
    return (a <= n && n <= b) || (a >= n && n >= b);
  }
  template <class T>
  T PMod(T x, T m) {
    while(x>=m)x-=m;
    while(x<0)x+=m;
    return x;
  }
  /*
  template <>
  inline int PMod<int>(int x, int m) {
    int wtr(x%m);
    return wtr<0 ? wtr+m : wtr;
  }
  */
  template <class T>
  T PModDiff(T n1, T n2, T limit) {
    n1 -= n2;
    while(n1>limit/2)n1-=limit;
    while(n1<-limit/2)n1+=limit;
    return n1;
  }
  template <class T>
  void swap(T& a, T& b) {
    char tmp[sizeof(T)];
    memcpy(tmp,&a,sizeof(T));
    memcpy(&a,&b,sizeof(T));
    memcpy(&b,tmp,sizeof(T));
  }
}

#endif

