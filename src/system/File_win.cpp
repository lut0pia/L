#include "File.h"

using namespace L;

bool File::exists() const {
  return (GetFileAttributes(_path) != INVALID_FILE_ATTRIBUTES);
}
void File::make() const {
  if(!exists())
    System::call("mkdir \""+_path+"\"");
}
bool File::mtime(const char* path, Date& date) {
  bool success(false);
  HANDLE handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
  if(handle != INVALID_HANDLE_VALUE) {
    FILETIME ft;
    if(GetFileTime(handle, nullptr, nullptr, &ft)) {
      SYSTEMTIME st;
      if(FileTimeToSystemTime(&ft, &st)) {
        struct tm tm;
        tm.tm_sec = st.wSecond;
        tm.tm_min = st.wMinute;
        tm.tm_hour = st.wHour;
        tm.tm_mday = st.wDay;
        tm.tm_mon = st.wMonth - 1;
        tm.tm_year = st.wYear - 1900;
        tm.tm_isdst = -1;
        date = Date(_mkgmtime(&tm));
        success = true;
      }
    }
    CloseHandle(handle);
  }
  return success;
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
