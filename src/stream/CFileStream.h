#pragma once

#include <cstdio>
#include "DirectStream.h"

namespace L {
  class CFileStream : public DirectStream {
  protected:
    FILE* _fd;
  public:
    inline CFileStream(FILE* fd = ::tmpfile()) : _fd(fd) {}
    inline CFileStream(const char* filepath, const char* mode) : _fd(fopen(filepath, mode)) {}
    inline ~CFileStream() { if(_fd)fclose(_fd); }

    inline size_t write(const void* data, size_t size) override { return fwrite(data, 1, size, _fd); }
    inline size_t read(void* data, size_t size) override { return fread(data, 1, size, _fd); }
    inline char get() override { return char(fgetc(_fd)); }
    inline void put(char c) override { fputc(c, _fd); }
    inline bool end() override { return feof(_fd)!=0; }

    inline char peek() override { char c(char(fgetc(_fd))); ungetc(c, _fd); return c; }
    inline void rewind() override { ::rewind(_fd); }
    inline uintptr_t tell() override { return ::ftell(_fd); }
    inline void seek(uintptr_t i) override { ::fseek(_fd, long(i), SEEK_SET); }
    inline size_t size() override {
      uintptr_t o(tell());
      ::fseek(_fd, 0, SEEK_END);
      uintptr_t r(tell());
      seek(o);
      return r;
    }
    inline void flush() { fflush(_fd); }

    operator bool() const { return _fd != nullptr; }
  };
  static CFileStream tmpfile, in(stdin), out(stdout), err(stderr);
}
