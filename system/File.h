#ifndef DEF_L_System_File
#define DEF_L_System_File

#include <fstream>
#include "../stl/String.h"

namespace L{
    class File{
        protected:
            String path;
        public:
            File(const String& path);
            bool operator<(const File&) const;

            String name() const;
            String dir() const;
            inline String gPath() const{return path;}

            void out(std::ofstream&, std::ios_base::openmode mode = std::ios_base::out) const;
            void in(std::ifstream&, std::ios_base::openmode mode = std::ios_base::in) const;
            bool exists() const;
            void makePath() const;
    };
}

#endif



