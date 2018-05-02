#include "Resource.h"

#include "../container/Buffer.h"
#include "../stream/CFileStream.h"

using namespace L;

Symbol ResourceSlotGeneric::parameter(const char* key) {
  const size_t key_length(strlen(key));
  const char* pair(id);
  while(true) {
    pair = strstr(pair+1, key); // Find all occurences of the key
    if(!pair) break; // Couldn't find the key
    if(pair[-1]!='?' && pair[-1]!='&') continue; // Make sure it's prefixed with ? or &
    if(pair[key_length]!='=') continue; // Make sure it's suffixed with =
    const char* value(pair+key_length+1);
    const size_t value_length(strcspn(value, "&\0"));
    return Symbol(value, value_length);
  }
  return Symbol();
}
Buffer ResourceSlotGeneric::read_source_file() {
  CFileStream stream(path, "rb");
  const size_t size(stream.size());
  Buffer wtr(size);
  stream.read(wtr, size);
  return wtr;
}
