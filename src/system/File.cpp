#include "File.h"

#if defined L_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#endif

#include "../macros.h"
#include "System.h"

using namespace L;

File::File(const char* path) : _path(System::formatPath(path)) {}

bool File::operator<(const File& other) const {
  return _path < other._path;
}

String File::name() const {
  return System::pathFile(_path);
}
String File::dir() const {
  return System::pathDirectory(_path);
}

bool File::exists() const {
#if defined L_WINDOWS
  return (GetFileAttributes(_path) != INVALID_FILE_ATTRIBUTES);
#elif defined L_UNIX
  return System::call("test -e \""+_path+"\"");
#endif
}
void File::make() const {
  if(!exists())
    System::call("mkdir \""+_path+"\"");
}
