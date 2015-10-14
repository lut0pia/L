#include "File.h"

#if defined L_WINDOWS
#include <windows.h>
#else
#include <fstream>
#include <sys/types.h>
#endif

#include "Directory.h"
#include "../macros.h"
#include "System.h"

using namespace L;
using namespace std;

File::File(const String& p) : _path(System::formatPath(p)), _fd(NULL) {}
File::~File() {
  close();
}
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
  WIN32_FIND_DATA fdFile;
  if(FindFirstFile(_path, &fdFile)                     // File found
      && !(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)    // Not a directory
      && !strcmp(fdFile.cFileName,name())) {              // Case sensitive same name
    return true;
  } else return false;
#elif defined L_UNIX
  return ifstream(path);
#endif
}
void File::makePath() const {
  Directory(dir()).make();
}

File& File::open(const char* mode) {
  makePath();
  close();
  _fd = fopen(_path,mode);
  return *this;
}
void File::close() {
  if(_fd!=NULL) {
    fclose(_fd);
    _fd = NULL;
  }
}
bool File::error() const {
  return ferror(_fd);
}

size_t File::read(char* buffer, int count) {
  return fread(buffer,1,count,_fd);
}
String File::read(int count) {
  String wtr(count,'\0');
  wtr.size(read(wtr,count));
  return wtr;
}
String File::readLine() {
  String wtr;
  char c;
  while(true) {
    c = fgetc(_fd);
    if(c==EOF) break;
    if(c=='\n') {
      if(wtr.size()) return wtr;
      else continue;
    } else wtr.push(c);
  }
  return wtr;
}
String File::readAll() {
  String wtr;
  size_t c;
  do {
    c = wtr.size();
    wtr += read((c)?c:16);
  } while(c<wtr.size());
  return wtr;
}

size_t File::write(const char* buffer, int count) {
  return fwrite(buffer,1,count,_fd);
}
void File::write(const String& str) {
  write(str,str.size());
}

