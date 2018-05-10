#pragma once

#include "String.h"
#include "../container/Array.h"

namespace L {
  // Conversions from utf8
  uint32_t utf8_to_utf32(const char*& str); // Returns code point for pointed character and increments pointer to next character
  uint32_t utf8_to_utf32(const char* str, int* size); // Returns first utf32 char from utf8 string and puts its byte size in size
  Array<uint32_t> utf8_to_utf32_array(const char* str);

  // Conversions to utf8
  String ansi_to_utf8(String); // Convert ANSI characters to utf8
  const char* utf16_to_utf8(uint16_t); // Returns utf8 string from utf16 char
  const char* utf32_to_utf8(uint32_t); // Returns utf8 string from utf32 char

  String url_encode(const String&);
  String url_decode(const String&);
}
