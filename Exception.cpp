#include "Exception.h"

#include "streams/Stream.h"

using namespace L;

Exception::Exception(const char* w) : w(w) {
  out << "Exception: " << w << '\n';
}
