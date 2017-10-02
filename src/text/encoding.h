#pragma once

#include "../types.h"
#include "String.h"
#include "../container/Array.h"

namespace L {
  const char* UTF16toUTF8(uint16_t); // Returns UTF-8 string from UTF-16 char
  uint32_t UTF8toUTF32(const char* str, int* size); // Returns UTF-32 char (s being UTF-8) and puts its size in size
  inline uint32_t ReadUTF8(const char*& str) { // Returns code point for pointed character and increments pointer to next character
    uint32_t wtr((byte)*str);
    if(wtr>>5==0x6) { // 2 bytes
      wtr <<= 6;
      wtr |= ((byte)*(++str)) & bitmask(6);
      wtr &= bitmask(11);
    } else if(wtr>>4==0xE) { // 3 bytes
      wtr <<= 6;
      wtr |= ((byte)*(++str)) & bitmask(6);
      wtr <<= 6;
      wtr |= ((byte)*(++str)) & bitmask(6);
      wtr &= bitmask(16);
    }
    str++;
    return wtr;
  }
  Array<uint32_t> UTF8toUTF32(const char* str);
  String ANSItoUTF8(String); // Convert ANSI characters to UTF-8

  String url_encode(const String&);
  String url_decode(const String&);
}
