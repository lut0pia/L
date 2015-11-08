#ifndef DEF_L_encoding
#define DEF_L_encoding

#include "../types.h"
#include "../String.h"
#include "../containers/Array.h"

namespace L {
  const char* UTF16toUTF8(uint16); // Returns UTF-8 string from UTF-16 char
  uint32 UTF8toUTF32(const char* str, int* size); // Returns UTF-32 char (s being UTF-8) and puts its size in size
  Array<uint32> UTF8toUTF32(const char* str);
  String ANSItoUTF8(String); // Convert ANSI characters to UTF-8
}
#endif
