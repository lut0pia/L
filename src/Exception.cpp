#include "Exception.h"

#include "streams/FileStream.h"

using namespace L;

Exception::Exception(const char* w) : w(w) {
  out << "Exception: " << w << '\n';
}
