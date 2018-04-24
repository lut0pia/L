#pragma once

#include <type_traits>
#include "../stream/Stream.h"
#include "../system/Memory.h"

namespace L {
  struct RefMeta {
    uint32_t counter, size;
  };
  template<class T>
  class Ref {
  private:
    static const size_t offset = sizeof(RefMeta);
    T* _p;
    inline RefMeta& meta() { return *((RefMeta*)_p-1); }
    inline uint32_t& counter(){ return meta().counter; }
    inline uint32_t& size() { return meta().size; }

  public:
    constexpr Ref() : _p(nullptr) {}
    constexpr Ref(nullptr_t) : _p(nullptr) {}
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
        Memory::free((uint8_t*)_p-offset,size()+offset);
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
      if(_p != other._p) {
        this->~Ref();
        new(this)Ref(other);
      }
      return *this;
    }
    template <typename... Args>
    T& make(Args&&... args){
      this->~Ref();
      _p = (T*)((uint8_t*)Memory::alloc(sizeof(T)+offset)+offset);
      counter() = 1;
      size() = sizeof(T);
      ::new(_p)T(args...);
      return **this;
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
