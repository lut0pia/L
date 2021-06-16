#include "Symbol.h"

#include "../container/Table.h"
#include "../dev/debug.h"
#include "../parallelism/Lock.h"
#include "../stream/serial_bin.h"
#include "../system/Memory.h"
#include "../macros.h"

using namespace L;

static const size_t blob_size = 2*1024*1024;
static Table<uint32_t, const char*> _symbols;
static char *_blob_next, *_blob_end;
static Lock symbol_lock;

Symbol::Symbol(const char* str, size_t length) {
  L_SCOPED_LOCK(symbol_lock);
  const uint32_t hash(fnv1a(str, length));
  if(const char** found = _symbols.find(hash))
    _string = *found;
  else {
    if(size_t(_blob_end)-size_t(_blob_next)<=(length+1)) {
      _blob_next = (char*)Memory::virtual_alloc(blob_size);
      _blob_end = _blob_next+blob_size;
    }
    memcpy(_blob_next, str, length);
    _blob_next[length] = '\0';
    _string = _blob_next;
    _symbols[hash] = _string;
    _blob_next += length+1;
  }
}

Stream& L::operator<=(Stream& s, const Symbol& v) {
  const uint16_t len(uint16_t(v._string ? strlen(v) : 0));
  s <= len;
  if(len>0) {
    s.write(v, len);
  }
  return s;
}
Stream& L::operator>=(Stream& s, Symbol& v) {
  char buffer[256];
  uint16_t len;
  s >= len;
  if(len>0) {
    s.read(buffer, len);
    v = Symbol(buffer, len);
  } else {
    v = Symbol();
  }
  return s;
}
