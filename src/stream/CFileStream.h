#pragma once

#include <cstdio>
#include "FileStream.h"

namespace L {
  class CFileStream : public FileStream {
  protected:
    FILE* _fd;
  public:
    inline CFileStream(FILE* fd = ::tmpfile()) : _fd(fd) {}
    inline CFileStream(const char* filepath, const char* mode) : _fd(fopen(filepath, mode)) {}
    inline ~CFileStream() { if(_fd)fclose(_fd); }

    inline size_t write(const void* data, size_t size) override { return fwrite(data, 1, size, _fd); }
    inline size_t read(void* data, size_t size) override { return fread(data, 1, size, _fd); }
    inline char get() override { return fgetc(_fd); }
    inline void put(char c) override { fputc(c, _fd); }
    inline void unget(char c) override { ungetc(c, _fd); }
    inline bool end() const override { return feof(_fd)!=0; }

    inline void rewind() override { ::rewind(_fd); }
    inline int tell() const override { return ::ftell(_fd); }
    inline void seek(int i) override { ::fseek(_fd, i, SEEK_SET); }
    inline size_t size() override {
      int o(tell());
      ::fseek(_fd, 0, SEEK_END);
      int r(tell());
      seek(o);
      return r;
    }
    inline void flush() { fflush(_fd); }

    operator bool() const { return _fd != nullptr; }
  };
  static CFileStream tmpfile, in(stdin), out(stdout), err(stderr);
}
