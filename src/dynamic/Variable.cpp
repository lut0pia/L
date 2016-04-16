#include "Variable.h"

using namespace L;

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

Variable::Variable(const char* s) : _td(Type<String>::description()) {
  if(local())
    new(&_data) String(s);
  else _p = new String(s);
}
Variable::Variable(const Variable& other) : _td(other._td) {
  if(local()) // Value is to be contained locally
    _td->cpyto(value(),other.value());
  else _p = _td->cpy(other._p);
}
Variable& Variable::operator=(const Variable& other) {
  if(this != &other) {
    if(_td==other._td) // Same type
      _td->assign(value(),other.value()); // Simple assignment
    else{ // Different type
      this->~Variable();
      new(this) Variable(other);
    }
  }
  return *this;
}
Variable::~Variable() {
  if(local()) // Value is contained locally
    _td->dtr(&_data);
  else _td->del(_p); // Value has been dynamically allocated
}

Variable& Variable::operator[](const L::String& str) {
  if(!is<Map<String,Variable> >()) *this = Map<String,Variable>();
  return as<Map<String,Variable> >()[str];
}
const Variable& Variable::operator[](const L::String& str) const {
  return as<Map<String,Variable> >()[str];
}
Variable& Variable::operator[](size_t i) {
  if(!is<Array<Variable> >()) *this = Array<Variable>();
  if(as<Array<Variable> >().size()<=i) as<Array<Variable> >().size(i+1);
  return as<Array<Variable> >()[i];
}

Stream& L::operator<<(L::Stream& s, const Variable& v) {
  v.type()->out(s,v.value());
  return s;
}
