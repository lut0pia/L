#ifndef DEF_L_math
#define DEF_L_math

namespace L {
  template <int base=10,class T>
  const char* numberToString(T v, int pad=0) {
    static char buffer[128] = {0};
    char* wtr(buffer+127);
    // Check for negativity
    bool negative;
    if(v<0) {
      negative = true;
      v = -v;
    } else negative = false;
    // Check for point
    int point(0);
    if(T((int)v)!=v) {
      while(T((int)v)!=v) {
        v *= base;
        point++;
        if(point<0) break;
      }
    }
    // Convert to int and stringify
    int i(v);
    if(!i) *--wtr = '0';
    else {
      while(pad>0 || i>0 || point>0) {
        char c('0'+(i%base));
        if(c>'9') c += 'a'-'9'-1;
        *--wtr = c;
        if(--point==0) *--wtr = '.';
        i /= base;
        pad--;
      }
      if(negative) *--wtr = '-'; // Add minus sign if negative
    }
    return wtr;
  }
  template <int base=10> inline const char* numberToString(double v, int pad = 0) {return numberToString<base,float>(v,pad);}
  template <int base=10,class T>
  T stringToNumber(const char* str) {
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
  template <class T> inline T log(const T& x, const T& base) {return log(x)/log(base);}
  template <class T> inline const T& max(const T& a, const T& b) {return (a>b)?a:b;}
  template <class T> inline const T& min(const T& a, const T& b) {return (a<b)?a:b;}
  template <class T> inline const T& clamp(const T& v, const T& min, const T& max) {return (v<min)?min:((v>max)?max:v);}
  template <class T> inline T abs(const T& n) {return (n<0)?-n:n;}

  template <class T> T pmod(T x, const T& m) {
    while(x>=m)x-=m;
    while(x<0)x+=m;
    return x;
  }
  template <> inline int pmod(int x, const int& m) {
    int wtr(x%m);
    return (wtr<0)?wtr+m:wtr;
  }
}

#endif




