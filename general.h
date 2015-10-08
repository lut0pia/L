#ifndef DEF_L_general
#define DEF_L_general

#include <cstring>
#include <cmath>
#include "types.h"
#include "String.h"
#include "streams/FileStream.h"

namespace L {
  // Number functions :
  template <class T>
  inline T log(T x, T base) {
    return ::log(x)/::log(base);
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
  String ToString(const T& t) {
    tmpfile << t;
    String wtr;
    wtr.size(tmpfile.tell());
    tmpfile.rewind();
    tmpfile.read(&wtr[0],wtr.size());
    tmpfile.rewind();
    return wtr;
  }
  template <class T>
  T FromString(const String& str) {
    T wtr;
    tmpfile << str << '\0';
    tmpfile.rewind();
    tmpfile >> wtr;
    tmpfile.rewind();
    return wtr;
  }
}

#endif

