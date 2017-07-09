#include "Symbol.h"

#include "../stream/serial.h"

using namespace L;

Table<const char*, const char*> Symbol::_symbols;
char *Symbol::_blobNext, *Symbol::_blobEnd;

Stream& L::operator<(Stream& s, const Symbol& sym) {
  size_t size(strlen(sym));
  s << size << ' ';
  s.write(sym, size);
  return s << '\n';
}
Stream& L::operator>(Stream& s, Symbol& sym) {
  size_t size;
  s > size;
  char* buffer((char*)Memory::alloc(size));
  s.read(buffer, size);
  buffer[size] = '\0';
  sym = buffer;
  Memory::free(buffer, size);
  return s;
}
