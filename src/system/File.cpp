#include "File.h"

#if defined L_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#endif

using namespace L;

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
