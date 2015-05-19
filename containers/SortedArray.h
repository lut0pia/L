#ifndef DEF_L_SortedArray
#define DEF_L_SortedArray

#include "Array.h"

namespace L {
  template <class T>
  class SortedArray : private Array<T> {
    private:
      int index(const T& e) {
        int wtr(0);
        while(wtr<Array<T>::size() && Array<T>::operator[](wtr)<e)
          wtr++;
        return wtr;
      }
    public:
      void insert(const T& e) {
        Array<T>::insert(index(e),e);
      }
      void erase(const T& e) {
        int i(index(e));
        if(i<Array<T>::size() && Array<T>::operator[](i)==e)
          Array<T>::erase(i);
      }
      inline const T& operator[](int i) const {return Array<T>::operator[](i);}
      inline void clear() {Array<T>::clear();}
      inline size_t size() const {return Array<T>::size();}
      inline void foreach(const std::function<void(const T&)>& f) const {Array<T>::foreach(f);}
  };
}

#endif

