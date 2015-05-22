#include "Directory.h"

#if defined L_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#endif

//#include <dirent.h>
#include "../Exception.h"
#include "../stl.h"
#include "System.h"

using namespace L;

const Directory Directory::current;
const Directory Directory::root("/");

Directory::Directory() {
#if defined L_WINDOWS
  path = System::callGet("cd");
#elif defined L_UNIX
  path = System::callGet("pwd");
#endif
  path[path.size()-1] = System::slash; // Because there's a \n at the end
}
Directory::Directory(const String& p) : path(System::formatPath(p)) {
  if(path.size()>1 && path[path.size()-1]!=System::slash)
    path += System::slash;
}
bool Directory::operator<(const Directory& other) const {
  return path < other.path;
}

String Directory::name() const {
#if defined L_WINDOWS
  if(path.size()>3) {
    String wtr(path.substr(1+path.find_last_of('\\',path.size()-2)));
    return wtr.substr(0,wtr.size()-1);
  } else return path.substr(0,2); // C:
#elif defined L_UNIX
  if(path.size()>1) {
    String wtr(path.substr(1+path.find_last_of('\\',path.size()-2)));
    return wtr.substr(0,wtr.size()-1);
  } else throw Exception("Directory::name() wait what?");
#endif
}
void Directory::goUp() {
#if defined L_WINDOWS
  if(path.size()>3)
    path.erase(1+path.find_last_of('\\',path.size()-2));
#elif defined L_UNIX
  if(path.size()>1)
    path.erase(1+path.find_last_of('/',path.size()-2));
#endif
  else goRoot();
}
void Directory::goDown(String child) {
  path += child + System::slash;
}
void Directory::goRoot() {
#if defined L_WINDOWS
  path = "\\"; // Will enumerate disks in content
#elif defined L_UNIX
  path = "/";
#endif
}

Directory::Content Directory::content() const {
  Content wtr;
#if defined L_WINDOWS
  if(path!="\\") { // Normal path
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    if((hFind = FindFirstFile((path+"*").c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
      throw Exception("Could not open directory: " + path);
    else {
      do {
        if(strcmp(fdFile.cFileName, ".")&&strcmp(fdFile.cFileName, "..")) { // No shit given about these two
          if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  // It's a directory
            if(!(fdFile.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
              wtr.directories.insert(Directory(path+String(fdFile.cFileName)));
          } else                                                  // It's a file
            wtr.files.insert(path+String(fdFile.cFileName));
        }
      } while(FindNextFile(hFind, &fdFile));
    }
    FindClose(hFind);
  } else { // Enumerate disks
    DWORD disks = GetLogicalDrives();
    char name[] = "A:";
    while(*name<='Z') {
      if(disks&0x1)
        wtr.directories.insert(Directory(name));
      (*name)++;
      disks >>= 1;
    }
  }
#elif defined L_UNIX
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(path.c_str())) != NULL) {
    while((dirp = readdir(dp)) != NULL) {
      switch(dirp->d_type) {
        case DT_REG:
          wtr.files.insert(File(dirp->d_name));
          break;
        case DT_DIR:
          wtr.directories.insert(String(dirp->d_name));
          break;
      }
    }
    closedir(dp);
  }
#endif
  return wtr;
}
bool Directory::exists() {
#if defined L_WINDOWS
  DWORD dwAttrib = GetFileAttributes(path.c_str());
  return (dwAttrib != INVALID_FILE_ATTRIBUTES
          && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined L_UNIX
  return System::call("test -d \""+path+"\"");
#endif
}
void Directory::make() {
  if(!exists()) {
#if defined L_WINDOWS
    System::call("mkdir \""+path+"\"");
#elif defined L_UNIX
    System::call("mkdir \""+path+"\"");
#endif
  }
}
