#pragma once

#include <cstring>
#include "../math/digits.h"

namespace L {
  class Stream {
  public:
    virtual size_t write(const void* data,size_t size) = 0;
    virtual size_t read(void* data,size_t size) = 0;
    virtual char get() { char c; read(&c,1); return c; };
    virtual void put(char c) { write(&c,1); }
    virtual void unget(char c) = 0;
    virtual bool end() const { return false; }

    inline char peek() { char c(get()); unget(c); return c; }
    inline void ignore(int n = 1) { while(n--)get(); }

    const char* line(); // Reads a line until \n
    const char* word(); // Reads a word until a space
    const char* bufferize(size_t*); // Reads as many chars as possible

    // Default operators
    template<class T> inline Stream& operator<<(const T& v) { write("N/A",3); return *this; }
    template<class T> inline Stream& operator>>(T& v) { return *this; }
    template<class T> inline Stream& operator<(const T& v) { write(&v,sizeof(v)); return *this; }
    template<class T> inline Stream& operator>(T& v) { read(&v,sizeof(v)); return *this; }

    template<class T> inline Stream& operator<<(T* v) { return *this << ntos<16>((uintptr_t)v,sizeof(v)*2); }

    inline Stream& operator<<(bool v) { if(v) write("true",4); else write("false",5); return *this; }
    inline Stream& operator<<(char v) { put(v); return *this; }
    inline Stream& operator<<(char* v) { write(v,strlen(v)); return *this; }
    inline Stream& operator<<(unsigned char v) { return *this << ntos<16>((unsigned int)v,2); }
    inline Stream& operator<<(const char* v) { write(v,strlen(v)); return *this; }
    inline Stream& operator<<(short v) { return *this << ntos(v); }
    inline Stream& operator<<(int v) { return *this << ntos(v); }
    inline Stream& operator<<(unsigned int v) { return *this << ntos(v); }
    inline Stream& operator<<(long long v) { return *this << ntos(v); }
    inline Stream& operator<<(unsigned long long v) { return *this << ntos(v); }
    inline Stream& operator<<(float v) { return *this << ntos(v); }
    inline Stream& operator<<(double v) { return *this << ntos(v); }

    inline Stream& operator>>(char& v) { v = get(); return *this; }
    inline Stream& operator>>(char* v) { strcpy(v,word()); return *this; }
    inline Stream& operator>>(int& v) { v = ston<10,int>(word()); return *this; }
    inline Stream& operator>>(unsigned int& v) { v = ston<10,unsigned int>(word()); return *this; }
    inline Stream& operator>>(float& v) { v = ston<10,float>(word()); return *this; }
    inline Stream& operator>>(double& v) { v = ston<10,double>(word()); return *this; }

    void nospace() { char c; do c = get(); while(isspace(c)); unget(c); }
    static inline bool isspace(char c) { return c==' '||c=='\t'||c=='\n'||c=='\v'||c=='\f'||c=='\r'; }
  };
}
