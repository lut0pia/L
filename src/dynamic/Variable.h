#pragma once

#include "Type.h"

namespace L {
  class String;
  class Variable {
  private:
    union {
      uintptr_t _data[3];
      void* _p;
    };
    const TypeDescription* _td;

    inline void* value(){ return (local()) ? (void*)&_data : _p; }
    inline const void* value() const{ return (local()) ? (void*)&_data : _p; }
    template <class T>
    void* allocate(){
      this->~Variable(); // Destruct currently held value
      _td = Type<T>::description(); // Change current type
      if(!local()) _p = std::allocator<T>().allocate(1); // Allocate memory for placement
      return value();
    }

  public:
    inline Variable() : _td(Type<int>::description()) {}
    template <class T> Variable(const T& v) : _td(Type<T>::description()) {
      if(local())  // Value is to be contained locally
        new(&_data) T(v);
      else _p = new T(v);
    }
    Variable(const char*);
    Variable(const Variable&);
    Variable(Variable&&);
    Variable& operator=(const Variable&);
    Variable& operator=(Variable&&);
    ~Variable();

    inline const TypeDescription* type() const { return _td; }
    inline bool local() const { return _td->size<=sizeof(_data); }

    template <class T> inline bool is() const { return _td == Type<T>::description(); }
    template <class T> inline const T& as() const { return *(T*)value(); }
    template <class T> inline T& as() { return *(T*)value(); }

    inline bool canbe(TypeDescription* td) const { return (_td->cast(td) != nullptr); }
    template <class T> inline bool canbe() const { return canbe(Type<T>::description()); }

    template <class T> T& make(){
      this->~Variable(); // Destruct current
      _td = Type<T>::description(); // Change type description
      if(local()) new(&_data) T(); // Local construct if small enough type
      else _p = new T(); // Dynamic allocation if large type
      return as<T>();
    }
    template <class T> T& cast() {
      if(is<T>()) return as<T>(); // It's already the right type: return as-is
      else if(Cast cast = _td->cast(Type<T>::description())) { // Try to find cast 
        Var tmp;
        cast(tmp.allocate<T>(),value()); // Allocate memory for new type then cast current value to allocated memory
        swap(*this,tmp); // Swap so this becomes the casted type and the old value gets destructed with tmp
        return as<T>();
      } else{ // There's no cast available: default construct it
        this->~Variable(); // Destruct current
        _td = Type<T>::description(); // Change type description
        if(local()) new(&_data) T(); // Local construct if small enough type
        else _p = new T(); // Dynamic allocation if large type
        return as<T>();
      }
    }
    template <class T> T get() const {
      if(is<T>()) return as<T>();
      else if(Cast cast = _td->cast(Type<T>::description())) {
        Var tmp;
        cast(tmp.allocate<T>(),value());
        return tmp.as<T>();
      } else return T();
    }

    inline Variable& operator+=(const Variable& other){
      if(type()==other.type() && type()->add)
        type()->add(value(),other.value());
      return *this;
    }
    inline Variable& operator-=(const Variable& other){
      if(type()==other.type() && type()->sub)
        type()->sub(value(),other.value());
      return *this;
    }
    inline Variable& operator*=(const Variable& other){
      if(type()==other.type() && type()->mul)
        type()->mul(value(),other.value());
      return *this;
    }
    inline Variable& operator/=(const Variable& other){
      if(type()==other.type() && type()->div)
        type()->div(value(),other.value());
      return *this;
    }
    inline Variable& operator%=(const Variable& other){
      if(type()==other.type() && type()->mod)
        type()->mod(value(),other.value());
      return *this;
    }
    inline Variable operator+(const Variable& other) const{ Variable wtr(*this); wtr += other; return wtr; }
    inline Variable operator-(const Variable& other) const{ Variable wtr(*this); wtr -= other; return wtr; }
    inline Variable operator*(const Variable& other) const{ Variable wtr(*this); wtr *= other; return wtr; }
    inline Variable operator/(const Variable& other) const{ Variable wtr(*this); wtr /= other; return wtr; }
    inline Variable operator%(const Variable& other) const{ Variable wtr(*this); wtr %= other; return wtr; }

    inline bool operator==(const Variable& other) const { return (_td == other._td && _td->cmp) ? (_td->cmp(value(),other.value()) == 0) : false; }
    inline bool operator!=(const Variable& other) const { return !(*this == other); }
    inline bool operator>(const Variable& other) const { return (_td == other._td && _td->cmp) ? (_td->cmp(value(),other.value()) > 0) : (_td>other._td); }
    inline bool operator<(const Variable& other) const { return (_td == other._td && _td->cmp) ? (_td->cmp(value(),other.value()) < 0) : (_td<other._td); }
    inline bool operator>=(const Variable& other) const { return !(*this < other); }
    inline bool operator<=(const Variable& other) const { return !(*this > other); }

    Variable& operator[](const String&);
    const Variable& operator[](const String&) const;
    Variable& operator[](size_t);

    template<class T> inline operator T() { return get<T>(); }

    friend Stream& operator<<(Stream&,const Variable&);
  };
  Stream& operator<<(Stream&,const Variable&);
  typedef Variable Var;
}
