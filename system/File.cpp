#include "File.h"

#include "Directory.h"
#include "../macros.h"
#include "../stl.h"
#include "System.h"

#if defined L_WINDOWS
    #include <windows.h>
#else
    #include <fstream>
    #include <sys/types.h>
#endif

using namespace L;
using namespace std;

File::File(const String& p) : path(System::formatPath(p)){}
bool File::operator<(const File& other) const{
    return path < other.path;
}

String File::name() const{
    return System::pathFile(path);
}
String File::dir() const{
    return System::pathDirectory(path);
}

void File::out(std::ofstream& s, std::ios_base::openmode mode) const{
    makePath();
    s.open(path.c_str(),mode);
}
void File::in(std::ifstream& s, std::ios_base::openmode mode) const{
    makePath();
    s.open(path.c_str(),mode);
}
bool File::exists() const{
    #if defined L_WINDOWS
        WIN32_FIND_DATA fdFile;
        if(FindFirstFile(path.c_str(), &fdFile)                     // File found
        && !(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)    // Not a directory
        && !strcmp(fdFile.cFileName,name().c_str())){               // Case sensitive same name
            return true;
        }
        else return false;
    #elif defined L_UNIX
        return ifstream(path);
    #endif
}
void File::makePath() const{
    Directory(dir()).make();
}
