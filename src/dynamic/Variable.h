#pragma once

#include "Type.h"

namespace L {
  class Variable {
  private:
    union {
      uintptr_t _data[1];
      void* _p;
    };
    const TypeDescription* _td;

    inline void* value(){ return (local()) ? (void*)&_data : _p; }
    inline const void* value() const{ return (local()) ? (void*)&_data : _p; }
    template <class T> inline T* value() { return (T*)(local<T>() ? &_data : _p); }
    template <class T> inline const T* value() const { return (T*)(local<T>() ? &_data : _p); }

  public:
    typedef Variable Intermediate;
    inline Variable() : _td(Type<void>::description()) {}
    template <class T> Variable(const T& v) : _td(Type<T>::description()) {
      if(local<T>())  // Value is to be contained locally
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
    template <class T> static constexpr bool local() { return sizeof(T)<=sizeof(_data); }

    template <class T> inline bool is() const { return _td == Type<T>::description(); }
    template <class T> inline const T& as() const { return *value<T>(); }
    template <class T> inline T& as() { return *value<T>(); }
    template <class T> inline const T* try_as() const { return is<T>() ? value<T>() : nullptr; }
    template <class T> inline T* try_as() { return is<T>() ? value<T>() : nullptr; }

    inline bool canbe(const TypeDescription* td) const { return _td==td || _td->casts.find(td) != nullptr; }
    template <class T> inline bool canbe() const { return canbe(Type<T>::description()); }

    void make(const TypeDescription*);
    void cast(const TypeDescription* td,Cast cast);

    template <class T> T& make(){
      this->~Variable(); // Destruct current
      _td = Type<T>::description(); // Change type description
      if(local<T>()) new(&_data) T(); // Local construct if small enough type
      else _p = new T(); // Dynamic allocation if large type
      return as<T>();
    }
    template <class T> T& cast() {
      if(is<T>()) return as<T>(); // It's already the right type: return as-is
      else if(Cast cast = _td->casts.get(Type<T>::description(), nullptr)) { // Try to find cast
        T tmp;
        tmp.~T(); // Cast will construct over, we have to destruct, hopefully not too slow
        cast(&tmp, value()); // Cast current value to temporary variable
        *this = tmp; // Move casted value to this
        return as<T>();
      } else return make<T>(); // There's no cast available: default construct it
    }
    template <class T> T get() const {
      if(is<T>()) return as<T>(); // It's already the right type: return as-is
      else if(Cast cast = _td->casts.get(Type<T>::description(), nullptr)) { // Try to find cast
        T tmp;
        tmp.~T(); // Cast will construct over, we have to destruct, hopefully not too slow
        cast(&tmp,value()); // Cast current value to temporary variable
        return tmp; // Returns temporary casted value
      } else return T(); // Returns default constructed type
    }

    Variable& operator+=(const Variable&);
    Variable& operator-=(const Variable&);
    Variable& operator*=(const Variable&);
    Variable& operator/=(const Variable&);
    Variable& operator%=(const Variable&);
    void invert();

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

    template<class T> inline operator T() { return get<T>(); }

    friend inline Stream& operator<<(Stream& s,const Variable& v) {
      v.type()->print(s,v.value());
      return s;
    }
    friend Stream& operator<(Stream& s, const Variable& v);
    friend Stream& operator>(Stream& s, Variable& v);
    friend Stream& operator<=(Stream& s, const Variable& v);
    friend Stream& operator>=(Stream& s, Variable& v);
    friend inline uint32_t hash(const Variable& v) { return v.type()->hash(v.value()); }
    friend inline void resource_write(Stream& s, const Variable& v) { s <= v; }
    friend inline void resource_read(Stream& s, Variable& v) { s >= v; }
  };

  Stream& operator<(Stream& s, const Variable& v);
  Stream& operator>(Stream& s, Variable& v);
  Stream& operator<=(Stream& s, const Variable& v);
  Stream& operator>=(Stream& s, Variable& v);

  typedef Variable Var;
}
