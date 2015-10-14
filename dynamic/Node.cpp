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
  v.foreach([&first,&s](const KeyValue<String,Variable>& e) {
    (first)?first = false:s.put(',');
    s << e.key() << ':' << e.value();
  });
  s.put('}');
  return s;
}
