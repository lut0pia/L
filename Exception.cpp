#include "Exception.h"

#include "streams/Stream.h"

using namespace L;

Exception::Exception(const String& w) : w(w) {
  out << w << '\n';
}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() {
  return w.c_str();
}
