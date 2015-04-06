#include "Exception.h"

using namespace L;

Exception::Exception(const String& w) : w(w) {}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() {
  return w.c_str();
}
