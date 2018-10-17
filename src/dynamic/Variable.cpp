#include "Variable.h"

#include "../macros.h"
#include "../text/String.h"

using namespace L;

Variable::Variable(const char* s) : _td(Type<String>::description()) {
  if(local())
    new(&_data) String(s);
  else _p = new String(s);
}
Variable::Variable(const Variable& other) : _td(other._td) {
  if(local()) // Value is to be contained locally
    _td->cpyto(_data,other._data);
  else _p = _td->cpy(other._p);
}
Variable::Variable(Variable&& other) : _td(other._td) {
  memcpy(_data,other._data,sizeof(_data));
  other._td = Type<int>::description();
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
Variable& Variable::operator=(Variable&& other){
  if(this != &other) {
    this->~Variable();
    memcpy(this,&other,sizeof(Variable));
    other._td = Type<int>::description();
  }
  return *this;
}
Variable::~Variable() {
  if(local()) // Value is contained locally
    _td->dtr(&_data);
  else _td->del(_p); // Value has been dynamically allocated
}

void Variable::make(const TypeDescription* td) {
  if(_td==td) return; // It's already the right type
  this->~Variable();
  _td = td;
  if(local())
    _td->ctr(_data);
  else
    _p = _td->ctrnew();
}
void Variable::cast(const TypeDescription* td,Cast cast){
  if(_td==td) return; // It's already the right type
  uint8_t tmp[256];
  cast(tmp,value()); // Cast to temporary
  this->~Variable(); // Destruct currently held value
  _td = td; // Change current type
  if(!local()) _p = ::operator new(td->size); // Allocate memory for placement
  memcpy(value(),tmp,td->size); // Copy casted temporary to this
}

#define OPERATOR(op) {\
  if(type()==other.type() && type()->op) \
    type()->op(value(),other.value()); \
  else{ \
    Cast c; \
    if(type()->op && (c = other.type()->casts.get(_td, nullptr))){ /* This type has operator and other can become this type */\
      uint8_t tmp[256]; \
      c(tmp,other.value()); /* Cast other to tmp */\
      type()->op(value(),tmp); \
      type()->dtr(tmp); /* Destruct temporary value */\
    } else if(other.type()->op && (c = _td->casts.get(other.type(), nullptr))){ /* Other's type has operator and this can become other's type */\
      cast(other.type(),c); /* Cast this to other's type */\
      type()->op(value(),other.value()); \
    } \
  } \
  return *this; \
}
Variable& Variable::operator+=(const Variable& other) OPERATOR(add)
Variable& Variable::operator-=(const Variable& other) OPERATOR(sub)
Variable& Variable::operator*=(const Variable& other) OPERATOR(mul)
Variable& Variable::operator/=(const Variable& other) OPERATOR(div)
Variable& Variable::operator%=(const Variable& other) OPERATOR(mod)
void Variable::invert() {
  if(type()->inv) {
    type()->inv(value());
  }
}

Variable& Variable::operator[](const Variable& key) {
  if(!is<Table<Variable,Variable> >()) *this = Table<Variable,Variable>();
  return as<Table<Variable,Variable> >()[key];
}
const Variable& Variable::operator[](const Variable& key) const {
  auto slot(as<Table<Variable,Variable> >().find(key));
  if(slot) return slot->value();
  else error("Couldn't find key in Var table.");
}
Variable& Variable::operator[](size_t i) {
  if(!is<Array<Variable> >()) *this = Array<Variable>();
  if(as<Array<Variable> >().size()<=i) as<Array<Variable> >().size(i+1);
  return as<Array<Variable> >()[i];
}

Stream& L::operator<(Stream& s, const Variable& v) {
  s < v.type()->name;
  v.type()->out_text(s, v.value());
  return s;
}
Stream& L::operator>(Stream& s, Variable& v) {
  Symbol type_name;
  s > type_name;
  const TypeDescription* new_type(types[type_name]);
  v.make(new_type);
  v.type()->in_text(s, v.value());
  return s;
}
Stream& L::operator<=(Stream& s, const Variable& v) {
  s <= v.type()->name;
  v.type()->out_bin(s, v.value());
  return s;
}
Stream& L::operator>=(Stream& s, Variable& v) {
  Symbol type_name;
  s >= type_name;
  if(const TypeDescription* new_type = types[type_name]) {
    v.make(new_type);
    v.type()->in_bin(s, v.value());
  } else {
    v = Var();
  }
  return s;
}
