#ifndef DEF_L_FileStream
#define DEF_L_FileStream

#include <cstdio>
#include "Stream.h"

namespace L {
  class FileStream : public Stream {
    protected:
      FILE* _fd;
    public:
      inline FileStream(FILE* fd=::tmpfile()) : _fd(fd) {}
      inline FileStream(const char* filepath, const char* mode) : _fd(fopen(filepath,mode)) {}
      inline ~FileStream() {fclose(_fd);}

      inline size_t write(const void* data, size_t size) {return fwrite(data,1,size,_fd);}
      inline size_t read(void* data, size_t size) {return fread(data,1,size,_fd);}
      inline char get() {return fgetc(_fd);}
      inline void put(char c) {fputc(c,_fd);}
      inline void unget(char c) {ungetc(c,_fd);}
      inline bool end() const {return feof(_fd)!=0;}

      inline void rewind() {::rewind(_fd);}
      inline int tell() const {return ::ftell(_fd);}
      inline void flush() {fflush(_fd);}

  };
  static FileStream tmpfile, in(stdin), out(stdout), err(stderr);
}

#endif

