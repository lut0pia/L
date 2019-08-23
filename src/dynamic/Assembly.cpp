#include "Assembly.h"

using namespace L;

uint8_t *Assembly::_write(0), *Assembly::_limit;

void Assembly::load(void* tmp) {
  uint8_t* f((uint8_t*)tmp);
  do emit(*f);
  while(*f++!=0xc3);
}

void* Assembly::commit() const {
  if(!_write || _write+_assembly.size()>=_limit) { // Allocate if starting or not enough space
    _write = (uint8_t*)System::alloc(_page);
    _limit = _write+_page;
  }
  memcpy(_write,&_assembly[0],_assembly.size()); // Copy raw assembly
  void* wtr(_write); // Save return value
  _write += _assembly.size(); // Update write cursor
  return wtr;
}
