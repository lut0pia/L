#ifndef DEF_L_Stream
#define DEF_L_Stream

#include <cstdio>

namespace L {
  class Stream {
    protected:
      FILE* _fd;
    public:
      inline Stream(FILE* fd) : _fd(fd) {}

      inline void write(const void* data, size_t size) {fwrite(data,1,size,_fd);}
      inline void read(void* data, size_t size) {fread(data,1,size,_fd);}

      inline Stream& operator<<(char v) {fputc(v,_fd); return *this;}
      inline Stream& operator<<(const char* v) {fputs(v,_fd); return *this;}
      inline Stream& operator<<(int v) {fprintf(_fd,"%d",v); return *this;}
      inline Stream& operator<<(float v) {fprintf(_fd,"%f",v); return *this;}

      inline Stream& operator>>(char& v) {v = fgetc(_fd); return *this;}
      inline Stream& operator>>(char*& v) {fscanf(_fd,"%s",v); return *this;}
      inline Stream& operator>>(int& v) {fscanf(_fd,"%d",&v); return *this;}
      inline Stream& operator>>(float& v) {fscanf(_fd,"%f",&v); return *this;}

      template<class T> inline Stream& operator<(const T& v) {write(&v,sizeof(v)); return *this;}
      template<class T> inline Stream& operator>(T& v) {read(&v,sizeof(v)); return *this;}
  };
  static Stream in(stdin), out(stdout), err(stderr);
}

#endif

