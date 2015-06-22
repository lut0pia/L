#include "Node.h"

#include "../macros.h"

using namespace L;
using namespace Dynamic;

Node& Node::operator()(const String& s, const Variable& v) {
  operator[](s) = v;
  return *this;
}
Stream& L::Dynamic::operator<<(Stream& s, const Node& v) {
  bool first = true;
  s.put('{');
  L_Iter(v,it) {
    if(first) first = false;
    else      s.put(',');
    s << (*it).first << ':' << (*it).second;
  }
  s.put('}');
  return s;
}
