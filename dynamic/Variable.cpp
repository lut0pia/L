#include "Variable.h"

#include "Array.h"
#include "Node.h"

using namespace L::Dynamic;

Variable& Variable::operator=(const Variable& other) {
  if(this != &other) {
    td->del(p);
    p = other.td->cpy(other.p);
    td = other.td;
  }
  return *this;
}

bool Variable::operator==(const Variable& other) {
  if(td == other.td && td->hascmp())
    return (td->cmp(p,other.p) == 0);
  else
    return false;
}
bool Variable::operator!=(const Variable& other) {
  return !(*this == other);
}
bool Variable::operator>(const Variable& other) {
  if(td == other.td && td->hascmp())
    return (td->cmp(p,other.p) > 0);
  else
    return false;
}
bool Variable::operator<(const Variable& other) {
  if(td == other.td && td->hascmp())
    return (td->cmp(p,other.p) < 0);
  else
    return false;
}
bool Variable::operator>=(const Variable& other) {
  return !(*this < other);
}
bool Variable::operator<=(const Variable& other) {
  return !(*this > other);
}

Variable& Variable::operator[](const L::String& str) {
  if(!is<Node>()) *this = Node();
  return as<Node>()[str];
}
const Variable& Variable::operator[](const L::String& str) const {
  return as<Node>().at(str);
}
Variable& Variable::operator[](size_t i) {
  if(!is<Array>()) *this = Dynamic::Array();
  if(get<Array>().size()<=i) get<Array>().resize(i+1);
  return get<Array>()[i];
}

std::ostream& L::Dynamic::operator<<(std::ostream& s, const Variable& v) {
  v.td->out(s,v.p);
  return s;
}
