#pragma once

#include "SortedArray.h"

namespace L {
  template <class T>
  class Set : protected SortedArray<T> {
    public:
      inline Set() {}
      inline Set(const std::initializer_list<T>& il){
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
        for(int i(0); i<other.size(); i++)
          insert(other[i]);
        return *this;
      }
      void insert(const T& e) {
        int i(SortedArray<T>::index(e));
        if(i>=(int)Array<T>::size() || e<Array<T>::operator[](i))
          Array<T>::insert(i,e);
      }
      void erase(const T& e) {
        int i(SortedArray<T>::index(e));
        if(i<(int)Array<T>::size() && !(Array<T>::operator[](i)<e))
          Array<T>::erase(i);
      }
      bool has(const T& e) {
        return (Array<T>::size() && Array<T>::operator[](SortedArray<T>::index(e))==e);
      }
      inline const T& operator[](int i) const {return Array<T>::operator[](i);}
      inline void clear() {Array<T>::clear();}
      inline size_t size() const {return Array<T>::size();}
      inline bool empty() const {return Array<T>::empty();}
      using Array<T>::begin;
      using Array<T>::end;
  };

  template <class T> inline Stream& operator<<(Stream& s, const Set<T>& v) { return s << (const Array<T>&)v;}
}
