#include "Array.h"

#include "../macros.h"

using namespace L;
using namespace Dynamic;

Dynamic::Array& Dynamic::Array::operator()(const Variable& other) {
  push(other);
  return *this;
}
std::ostream& L::Dynamic::operator<<(std::ostream& s, const Dynamic::Array& v) {
  bool first = true;
  s.put('[');
  v.foreach([&first,&s](const Variable& var) {
    if(first) first = false;
    else      s.put(',');
    s << var;
  });
  s.put(']');
  return s;
}
