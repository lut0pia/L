#ifndef DEF_L_encoding
#define DEF_L_encoding

#include <cstdlib>
#include "../String.h"
#include "../containers/Array.h"

namespace L{
    size_t UTF8toUTF32(const char* s, size_t* n = NULL); // Returns UTF-32 char (s being UTF-8) and puts its size in n
    Array<size_t> UTF8toUTF32(const String&);
    String ANSItoUTF8(String); // Convert ANSI characters to UTF-8
}
#endif
