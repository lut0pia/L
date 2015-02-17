#ifndef DEF_L_System_Directory
#define DEF_L_System_Directory

#include "File.h"
#include "../macros.h"
#include "../stl.h"

namespace L{
    class Directory{
        public:
            typedef struct{
                Set<File> files;
                Set<Directory> directories;
            } Content;
        protected:
            String path;
        public:
            static const Directory current;
            static const Directory root;
            Directory();
            Directory(const String& path);
            bool operator<(const Directory&) const;

            String name() const;
            void goUp();
            void goDown(String);
            void goRoot();

            inline String gPath() const{return path;}

            Content content() const;
            bool exists();
            void make();
    };
}

#endif



