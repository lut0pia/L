#pragma once

#include "SortedArray.h"

namespace L {
  template <class T>
  class Set : protected SortedArray<T> {
  public:
    constexpr Set() {}
    inline Set(const std::initializer_list<T>& il) {
      Array<T>::growTo(il.size());
      for(auto&& e : il)
        insert(e);
    }
    Set operator+(const Set& other) {
      Set wtr;
      uint32_t i(0), j(0);
      while(i<size() || j<other.size()) {
        if(i==size() || (j<other.size() && other[j]<operator[](i)))
          wtr.push(other[j++]);
        else if(j==other.size() || (i<size() && other[j]>operator[](i)))
          wtr.push(operator[](i++));
      }
      return wtr;
    }
    Set& operator+=(const Set& other) {
      for(uintptr_t i(0); i<other.size(); i++)
        insert(other[i]);
      return *this;
    }
    void insert(const T& e) {
      const uintptr_t i(SortedArray<T>::index(e));
      if(i>=Array<T>::size() || e<Array<T>::operator[](i))
        Array<T>::insert(i, e);
    }
    void erase(const T& e) {
      const uintptr_t i(SortedArray<T>::index(e));
      if(i<Array<T>::size() && !(Array<T>::operator[](i)<e))
        Array<T>::erase(i);
    }
    inline bool has(const T& e) {
      return (Array<T>::size() && Array<T>::operator[](SortedArray<T>::index(e))==e);
    }
    using Array<T>::operator[];
    using Array<T>::clear;
    using Array<T>::size;
    using Array<T>::empty;
    using Array<T>::begin;
    using Array<T>::end;

    friend inline Stream& operator<<(Stream& s, const Set& v) { return s << (const Array<T>&)v; }
    friend inline Stream& operator<(Stream& s, const Set& v) { return s < (const Array<T>&)v; }
    friend inline Stream& operator>(Stream& s, Set& v) { return s > (Array<T>&)v; }
  };
}
