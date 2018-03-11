#include "Symbol.h"

#include "../stream/serial.h"

using namespace L;

Table<const char*, const char*> Symbol::_symbols;
char *Symbol::_blob_next, *Symbol::_blob_end;
