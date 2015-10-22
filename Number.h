#ifndef DEF_L_Number
#define DEF_L_Number

#include "types.h"

namespace L {
  namespace Number {
    template <int base=10,class T>
    const char* toString(T v, int pad=0) {
      static char buffer[128] = {0};
      char* wtr(buffer+127);
      while(pad>0 || v>0) {
        char c('0'+(v%base));
        if(c>'9') c += 'a'-'9'-1;
        *--wtr = c;
        v /= base;
        pad--;
      }
      return wtr;
    }
    template <int base=10,class T>
    T fromString(const char* str) {
      T wtr(0);
      bool negative(*str=='-' && *++str);
      int fract(0);
      char c;
      while((c=*str++)!='\0') {
        int v(0);
        if(c=='.') {fract=1; continue;}
        else if(c>='0'&&c<='9') v = c-'0';
        else if(c>='a'&&c<='z') v = c-'a'+10;
        else if(c>='A'&&c<='Z') v = c-'A'+10;
        wtr *= base;
        wtr += v;
        if(fract) fract *= base;
      }
      if(fract) wtr /= fract;
      return (negative)?-wtr:wtr;
    }
    template <int base=2,class T>
    const T& log(const T& v) {
    }
  };
}

#endif




