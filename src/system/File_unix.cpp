#include "File.h"

#include <unistd.h>
#include <sys/stat.h>

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
Array<String> File::list(const char* path) {
  String output;
  System::call(String("find ")+path+" -printf \"%p\\n\"", output);
  return output.explode('\n');
}
