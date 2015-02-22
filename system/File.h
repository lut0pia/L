#ifndef DEF_L_System_File
#define DEF_L_System_File

#include <fstream>
#include "../stl/String.h"

namespace L {
  class File {
    protected:
      String _path;
      FILE* _fd;
    public:
      File(const String& path);
      ~File();
      bool operator<(const File&) const;

      String name() const;
      String dir() const;
      inline const String& path() const {return _path;}

      bool exists() const;
      void makePath() const;

      File& open(const char* mode);
      void close();

      int read(char* buffer, int count);
      String read(int count);
      String readAll();

      int write(const char* buffer, int count);
      void write(const String&);
  };
}

#endif



