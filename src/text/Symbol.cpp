#include "Symbol.h"

#include "../stream/serial.h"

using namespace L;

Table<const char*, const char*> Symbol::_symbols;
char *Symbol::_blobNext, *Symbol::_blobEnd;
