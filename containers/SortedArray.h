#ifndef DEF_L_SortedArray
#define DEF_L_SortedArray

#include "Array.h"

namespace L {
  template <class T>
  class SortedArray : protected Array<T> {
    public:
      int index(const T& e) const {
        int l(0), r(size());
        while(l<r) {
          int m((l+r)/2);
          if(Array<T>::operator[](m)<e)
            l = m+1;
          else if(e<Array<T>::operator[](m))
            r = m;
          else return m;
        }
        return l;
      }
      int first(const T& e) {
        int i(index(e));
        if(operator[](i)<e)
          return -1;
        else while(i>0 && !(operator[](i-1)<e))
            i--;
        return i;
      }
      int last(const T& e) {
        int i(index(e));
        if(e<operator[](i))
          return -1;
        else while(i<size() && !(e<operator[](i)))
            i++;
        return i;
      }
      void insert(const T& e) {
        Array<T>::insert(index(e),e);
      }
      inline void erase(int i) {Array<T>::erase(i);}
      inline const T& operator[](int i) const {return Array<T>::operator[](i);}
      inline void clear() {Array<T>::clear();}
      inline size_t size() const {return Array<T>::size();}
      inline void foreach(const std::function<void(const T&)>& f) const {Array<T>::foreach(f);}
  };
}

#endif

