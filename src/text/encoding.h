#pragma once

#include "../types.h"
#include "String.h"
#include "../containers/Array.h"

namespace L {
  const char* UTF16toUTF8(uint16_t); // Returns UTF-8 string from UTF-16 char
  uint32_t UTF8toUTF32(const char* str, int* size); // Returns UTF-32 char (s being UTF-8) and puts its size in size
  Array<uint32_t> UTF8toUTF32(const char* str);
  String ANSItoUTF8(String); // Convert ANSI characters to UTF-8
}
