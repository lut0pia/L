#ifndef DEF_L_Set
#define DEF_L_Set

#include "SortedArray.h"

namespace L {
  template <class T>
  class Set : protected SortedArray<T> {
    public:
      Set operator+(const Set& other) {
        Set wtr;
        int i(0), j(0);
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
        int i(index(e));
        if(i>=Array<T>::size() || e<Array<T>::operator[](i))
          Array<T>::insert(i,e);
      }
      void erase(const T& e) {
        int i(index(e));
        if(i<Array<T>::size() && !(Array<T>::operator[](i)<e))
          Array<T>::erase(i);
      }
      bool has(const T& e) {
        return (Array<T>::size() && Array<T>::operator[](index(e))==e);
      }
      inline const T& operator[](int i) const {return Array<T>::operator[](i);}
      inline void clear() {Array<T>::clear();}
      inline size_t size() const {return Array<T>::size();}
      inline bool empty() const {return Array<T>::empty();}
      inline void foreach(const std::function<void(const T&)>& f) const {Array<T>::foreach(f);}
  };

  template <class T> inline Stream& operator<<(Stream& s, const Set<T>& v) { return s << (const Array<T>&)v;}
}

#endif

