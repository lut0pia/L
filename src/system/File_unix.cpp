#include "File.h"

using namespace L;

bool File::exists() const {
  return access(_path, F_OK) == 0;
}
void File::make() const {
  if(!exists())
    System::call("mkdir \""+_path+"\"");
}
bool File::mtime(Date& date) const {
  struct stat buf;
  if(!stat(_path, &buf)) {
    date = time_t(buf.st_mtim.tv_sec);
    return true;
  } else return false;
}
