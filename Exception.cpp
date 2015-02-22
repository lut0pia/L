#include "Exception.h"

using namespace L;

Exception::Exception(const String& w) : w(w) {
  if(w[w.size()-1]!='.')
    this->w.push_back('.');
}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() {
  return w.c_str();
}
