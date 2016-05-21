#pragma once

#include "../streams/Stream.h"

namespace L {
  template<class T>
  class Ref {
  private:
    struct Holder{
      int _references;
      byte _value[sizeof(T)];
    };
    Holder* _holder;
  public:
    inline Ref() : _holder(nullptr) {}
    template <typename... Args>
    inline Ref(const Args&... args) : _holder(new Holder) {
      _holder->_references = 1;
      new (_holder->_value)T(args...);
    }
    inline Ref(const Ref& other) {
      _holder = other._holder;
      _holder->_references++;
    }
    template <class R> inline Ref(const Ref<R>& other) {
      static_assert(std::is_base_of<T,R>::value,"Cannot convert to a non-base class");
      _holder = (Ref<T>::Holder*)other._holder;
      _holder->_references++;
    }
    inline ~Ref() {
      if(_holder && --_holder->_references==0){
        destruct(**this);
        delete _holder;
      }
    }
    Ref& operator=(const Ref& other) {
      if(_holder != other._holder) {
        destruct(*this);
        construct(*this,other);
      }
      return *this;
    }
    template <class R>
    Ref& operator=(const Ref<R>& other) {
      if((uintptr_t)_holder != (uintptr_t)other._holder) {
        destruct(*this);
        construct(*this,other);
      }
      return *this;
    }
    template <typename... Args>
    void make(const Args&... args){
      destruct(*this);
      _holder = new Holder;
      _holder->_references = 1;
      new (_holder->_value)T(args...);
    }
    template <class R> inline bool operator==(const Ref<R>& other) { return (uintptr_t)_holder==(uintptr_t)other._holder; }
    inline const T& operator*() const { return *((T*)&_holder->_value); }
    inline T& operator*() { return *((T*)&_holder->_value); }
    inline operator T*() const { return ((T*)&_holder->_value); }
    inline T* operator->() const { return ((T*)&_holder->_value); }
    inline bool null() const { return (_holder==nullptr); }
    inline void clear() { destruct(*this); _holder = nullptr; }
    inline int references() const { return (_holder) ? _holder->_references : 0; }
    template <class R> friend class Ref;
  };
  template <class T>
  Stream& operator<<(Stream& s,const Ref<T>& v) {
    return s << '(' << ((T*)v) << ',' << v.references() << ')';
  }
}
