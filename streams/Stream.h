#ifndef DEF_L_Stream
#define DEF_L_Stream

#include <cstdio>

namespace L {
  class Stream {
    protected:
      FILE* _fd;
    public:
      inline Stream(FILE* fd) : _fd(fd) {}

      inline size_t write(const void* data, size_t size) {return fwrite(data,1,size,_fd);}
      inline size_t read(void* data, size_t size) {return fread(data,1,size,_fd);}
      inline char peek() {char wtr(get()); unget(wtr); return wtr;}
      inline char get() {return fgetc(_fd);}
      inline void put(char c) {fputc(c,_fd);}
      inline void unget(char c) {ungetc(c,_fd);}
      inline void ignore(int n=1) {while(n--)get();}
      inline bool eof() const {return feof(_fd);}
      inline void flush(){fflush(_fd);}

      const char* line(); // Reads a line until \n

      // Default operators
      template<class T> inline Stream& operator<<(const T& v) {return operator<<("N/A");}
      template<class T> inline Stream& operator>>(T& v) {return *this;}
      template<class T> inline Stream& operator<(const T& v) {write(&v,sizeof(v)); return *this;}
      template<class T> inline Stream& operator>(T& v) {read(&v,sizeof(v)); return *this;}

      inline Stream& operator<<(bool v) {fputs((v)?"true":"false",_fd); return *this;}
      inline Stream& operator<<(char v) {fputc(v,_fd); return *this;}
      inline Stream& operator<<(char* v) {fputs(v,_fd); return *this;}
      inline Stream& operator<<(const char* v) {fputs(v,_fd); return *this;}
      inline Stream& operator<<(int v) {fprintf(_fd,"%i",v); return *this;}
      inline Stream& operator<<(unsigned int v) {fprintf(_fd,"%u",v); return *this;}
      inline Stream& operator<<(float v) {fprintf(_fd,"%f",v); return *this;}
      inline Stream& operator<<(double v) {fprintf(_fd,"%f",v); return *this;}
      inline Stream& operator<<(const void* v) {fprintf(_fd,"%p",v); return *this;}

      inline Stream& operator>>(char& v) {v = fgetc(_fd); return *this;}
      inline Stream& operator>>(char* v) {fscanf(_fd,"%s",v); return *this;}
      inline Stream& operator>>(int& v) {fscanf(_fd,"%i",&v); return *this;}
      inline Stream& operator>>(unsigned int& v) {fscanf(_fd,"%u",&v); return *this;}
      inline Stream& operator>>(float& v) {fscanf(_fd,"%f",&v); return *this;}
      inline Stream& operator>>(double& v) {fscanf(_fd,"%f",&v); return *this;}

      void nospace() {char tmp; while(isspace(tmp = get())) {} unget(tmp);}
      static inline bool isspace(char c) {return c==' '||c=='\t'||c=='\n'||c=='\v'||c=='\f'||c=='\r';}
  };
  static Stream in(stdin), out(stdout), err(stderr);
}

#endif

