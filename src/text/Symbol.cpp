#include "Symbol.h"

#include "../stream/serial.h"

using namespace L;

Table<uint32_t, const char*> Symbol::_symbols;
char *Symbol::_blob_next, *Symbol::_blob_end;
