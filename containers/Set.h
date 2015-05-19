#ifndef DEF_L_Set
#define DEF_L_Set

#include "Array.h"

namespace L {
  namespace Pending {
    template <class T>
    class Set : private Array<T> {
      private:
        int index(const T& e) {
          int wtr(0);
          while(wtr<Array<T>::size() && Array<T>::operator[](wtr)<e)
            wtr++;
          return wtr;
        }
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
        void insert(const T& e) {
          int i(index(e));
          if(i>=Array<T>::size() || Array<T>::operator[](i)!=e)
            Array<T>::insert(i,e);
        }
        void erase(const T& e) {
          int i(index(e));
          if(i<Array<T>::size() && Array<T>::operator[](i)==e)
            Array<T>::erase(i);
        }
        bool has(const T& e) {
          return (Array<T>::size() && Array<T>::operator[](index(e))==e);
        }
        inline const T& operator[](int i) const {return Array<T>::operator[](i);}
        inline void clear() {Array<T>::clear();}
        inline size_t size() const {return Array<T>::size();}
        inline void foreach(const std::function<void(const T&)>& f) const {Array<T>::foreach(f);}
    };
  }
}

#endif

