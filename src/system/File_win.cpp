#include "File.h"

#include <Windows.h>
#include <sys/stat.h>

using namespace L;

bool File::exists() const {
  return (GetFileAttributes(_path) != INVALID_FILE_ATTRIBUTES);
}
void File::make() const {
  if(!exists())
    System::call("mkdir \""+_path+"\"");
}
bool File::mtime(const char* path, Date& date) {
  struct _stat buf;
  if(!_stat(path, &buf)) {
    date = time_t(buf.st_mtime);
    return true;
  } else return false;
}
Array<String> File::list(const char* path) {
  Array<String> wtr;
  HANDLE handle;
  WIN32_FIND_DATA find_data;
  handle = FindFirstFile(path, &find_data);
  if(handle != INVALID_HANDLE_VALUE) {
    do {
      wtr.push(find_data.cFileName);
    } while(FindNextFile(handle, &find_data));
    FindClose(handle);
  }
  return wtr;
}
