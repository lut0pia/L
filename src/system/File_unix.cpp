#include "File.h"

using namespace L;

bool File::exists() const {
  return access(_path, F_OK) == 0;
}
void File::make() const {
  if(!exists())
    System::call("mkdir \""+_path+"\"");
}
bool File::mtime(const char* path, Date& date) {
  struct stat buf;
  if(!stat(path, &buf)) {
    date = time_t(buf.st_mtim.tv_sec);
    return true;
  } else return false;
}
