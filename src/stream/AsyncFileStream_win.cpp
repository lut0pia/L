#include "AsyncFileStream.h"

#include <windows.h>
#include "../macros.h"
#include "../parallelism/TaskSystem.h"
#include "CFileStream.h"

using namespace L;

struct DataStruct {
  HANDLE _handle;
  DWORD _pos;
};
#define _dataStruct (_data.as<DataStruct>())
#define _handle (_dataStruct._handle)
#define _pos (_dataStruct._pos)

AsyncFileStream::AsyncFileStream(const char* filepath, const char* mode) {
  // We're setting access and creation according to mode by mimicking fopen
  DWORD access(0);
  DWORD creation(OPEN_ALWAYS);
  for(; *mode; mode++)
    switch(*mode) {
      case 'w':
        access |= GENERIC_WRITE;
        break;
      case 'r':
        access |= GENERIC_READ;
        creation = OPEN_EXISTING;
        break;
      case 'a':
        access |= (GENERIC_READ | GENERIC_WRITE);
        break;
      case '+':
        access |= (GENERIC_READ | GENERIC_WRITE);
        break;
    }
  L_ASSERT(access!=0);
  _handle = CreateFile(filepath, access, 0, NULL, creation, FILE_FLAG_OVERLAPPED, NULL);
  L_ASSERT(_handle!=INVALID_HANDLE_VALUE);
  _pos = 0;
}
AsyncFileStream::~AsyncFileStream() {
  if(*this) CloseHandle(_handle);
}

size_t AsyncFileStream::write(const void* data, size_t size) {
  OVERLAPPED overlapped = {};
  overlapped.Offset = _pos;
  DWORD result(WriteFile(_handle, data, DWORD(size), NULL, &overlapped));
  L_ASSERT(result==FALSE && GetLastError()==ERROR_IO_PENDING);
  DWORD wtr;
  while(!GetOverlappedResult(_handle, &overlapped, &wtr, FALSE)) {
    out << "writing\n";
    TaskSystem::yield();
  }
  _pos += wtr;
  return wtr;
}
size_t AsyncFileStream::read(void* data, size_t size) {
  OVERLAPPED overlapped = {};
  overlapped.Offset = _pos;
  DWORD result(ReadFile(_handle, data, DWORD(size), NULL, &overlapped));
  L_ASSERT(result==FALSE && GetLastError()==ERROR_IO_PENDING);
  DWORD wtr;
  while(!GetOverlappedResult(_handle, &overlapped, &wtr, FALSE)) {
    out << "reading\n";
    TaskSystem::yield();
  }
  _pos += wtr;
  return wtr;
}
bool AsyncFileStream::end() {
  return false;
}

uintptr_t AsyncFileStream::tell() {
  return _pos;
}
void AsyncFileStream::seek(uintptr_t i) {
  L_ASSERT(i>>10<<10==i);
  _pos = i;
}
size_t AsyncFileStream::size() {
  union {
    size_t wtr;
    struct {
      DWORD low, high;
    };
  };
  low = GetFileSize(_handle, &high);
  return wtr;
}

AsyncFileStream::operator bool() const {
  return _handle != INVALID_HANDLE_VALUE;
}