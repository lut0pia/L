#include "Assembly.h"

using namespace L;

byte *Assembly::_write(0), *Assembly::_limit;

void Assembly::load(void* tmp) {
  byte* f((byte*)tmp);
  do emit(*f);
  while(*f++!=0xc3);
}

void* Assembly::commit() const {
  if(!_write || _write+_assembly.size()>=_limit) { // Allocate if starting or not enough space
    _write = (byte*)System::alloc(_page);
    _limit = _write+_page;
  }
  memcpy(_write,&_assembly[0],_assembly.size()); // Copy raw assembly
  void* wtr(_write); // Save return value
  _write += _assembly.size(); // Update write cursor
  return wtr;
}

Stream& L::operator<<(Stream& s, const Assembly& a) {
  return s << a._assembly;
}
