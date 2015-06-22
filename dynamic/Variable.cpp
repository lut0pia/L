#include "Variable.h"

#include "Array.h"
#include "Node.h"

using namespace L::Dynamic;

void* Variable::value() {
  if(local()) // Value is contained locally
    return (void*)&_data;
  else return _p; // Value has been dynamically allocated
}
const void* Variable::value() const {
  if(local()) // Value is contained locally
    return (void*)&_data;
  else return _p; // Value has been dynamically allocated
}

Variable::Variable(const Variable& other) : _td(other._td) {
  if(local()) // Value is to be contained locally
    _td->cpyto(value(),other.value());
  else _p = _td->cpy(other._p);
}
Variable& Variable::operator=(const Variable& other) {
  if(this != &other) {
    this->~Variable();
    new(this) Variable(other);
  }
  return *this;
}
Variable::~Variable() {
  if(_td->size<=sizeof(_data)) // Value is contained locally
    _td->dtr(&_data);
  else _td->del(_p); // Value has been dynamically allocated
}

bool Variable::operator==(const Variable& other) {
  if(_td == other._td && _td->hascmp())
    return (_td->cmp(_p,other._p) == 0);
  else
    return false;
}
bool Variable::operator!=(const Variable& other) {
  return !(*this == other);
}
bool Variable::operator>(const Variable& other) {
  if(_td == other._td && _td->hascmp())
    return (_td->cmp(_p,other._p) > 0);
  else
    return false;
}
bool Variable::operator<(const Variable& other) {
  if(_td == other._td && _td->hascmp())
    return (_td->cmp(_p,other._p) < 0);
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
  if(get<Array>().size()<=i) get<Array>().size(i+1);
  return get<Array>()[i];
}

L::Stream& L::Dynamic::operator<<(L::Stream& s, const Variable& v) {
  v._td->out(s,v.value());
  return s;
}
