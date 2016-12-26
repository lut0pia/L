#pragma once

#include <type_traits>
#include "../streams/Stream.h"
#include "../system/Memory.h"

namespace L {
  template<class T>
  class Ref {
  private:
    static const size_t offset = 8;
    T* _p;
    inline int& counter(){ return *((int*)_p-1); }

  public:
    inline Ref() : _p(nullptr) {}
    inline Ref(const Ref& other) {
      _p = other._p;
      if(_p) counter()++;
    }
    template <class R> inline Ref(const Ref<R>& other) {
      static_assert(std::is_base_of<T,R>::value,"Cannot convert to a non-base class");
      _p = other._p;
      if(_p) counter()++;
    }
    inline ~Ref() {
      if(_p && --counter()==0){
        _p->~T();
        Memory::free((byte*)_p-offset,sizeof(T)+offset);
      }
    }
    inline Ref& operator=(const Ref& other) {
      if(_p != other._p) {
        this->~Ref();
        new(this)Ref(other);
      }
      return *this;
    }
    template <class R>
    inline Ref& operator=(const Ref<R>& other) {
      if(_p != _p) {
        this->~Ref();
        new(this)Ref(other);
      }
      return *this;
    }
    template <typename... Args>
    void make(Args&&... args){
      this->~Ref();
      _p = (T*)((byte*)Memory::alloc(sizeof(T)+offset)+offset);
      counter() = 1;
      new(_p)T(args...);
    }
    template <class R> inline bool operator==(const Ref<R>& other) { return _p==other._p; }
    inline const T& operator*() const { return *_p; }
    inline T& operator*() { return *_p; }
    inline operator T*() const { return _p; }
    inline T* operator->() const { return _p; }
    inline bool null() const { return _p==nullptr; }
    inline void clear() { destruct(*this); _p = nullptr; }
    inline int counter() const { return (_p) ? *((int*)_p-1) : 0; }
    template <class R> friend class Ref;
  };
  template <class T,typename... Args>
  inline Ref<T> ref(Args&&... args){
    Ref<T> wtr;
    wtr.make(args...);
    return wtr;
  }
  template <class T>
  inline Stream& operator<<(Stream& s,const Ref<T>& v) { return (v.null()) ? s << "null" : s << *v; }
}
