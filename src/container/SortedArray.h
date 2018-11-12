#pragma once

#include "Array.h"

namespace L {
  template <class T>
  class SortedArray : protected Array<T> {
  public:
    inline SortedArray() {}
    inline SortedArray(const std::initializer_list<T>& il) {
      Array<T>::grow_to(il.size());
      for(auto&& e : il)
        insert(e);
    }
    template <class R>
    uintptr_t index(const R& e) const {
      uintptr_t l(0), r(size());
      while(l<r) {
        uintptr_t m((l+r)/2);
        if(Array<T>::operator[](m)<e)
          l = m+1;
        else if(Array<T>::operator[](m)>e)
          r = m;
        else return m;
      }
      return l;
    }
    template <class R>
    int first(const R& e) {
      int i(index(e));
      if(operator[](i)<e)
        return -1;
      else while(i>0 && !(operator[](i-1)<e))
        i--;
      return i;
    }
    template <class R>
    int last(const R& e) {
      int i(index(e));
      if(e<operator[](i))
        return -1;
      else while(i<size() && !(e<operator[](i)))
        i++;
      return i;
    }
    inline void insert(const T& e) { Array<T>::insert(index(e), e); }

    using Array<T>::erase;
    using Array<T>::operator[];
    using Array<T>::clear;
    using Array<T>::size;
    using Array<T>::begin;
    using Array<T>::end;

    friend inline Stream& operator<<(Stream& s, const SortedArray& v) { return s << (const Array<T>&)v; }
    friend inline Stream& operator<(Stream& s, const SortedArray& v) { return s < (const Array<T>&)v; }
    friend inline Stream& operator>(Stream& s, SortedArray& v) { return s > (Array<T>&)v; }
  };
}
