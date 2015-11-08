#ifndef DEF_L_MultiArray
#define DEF_L_MultiArray

#include "Array.h"
#include "../geometry/Vector.h"

namespace L {
  template <int d,class T>
  class MultiArray : private Array<T> {
    protected:
      Vector<d,int> _size;
    public:
      inline MultiArray() : Array<T>(), _size(0) {}
      template <typename... Args>
      inline MultiArray(int i, Args&&... args) : Array<T>() {
        resizeFast(Vector<d,int>(i,args...));
      }
      template <typename... Args>
      inline MultiArray(const T* a, int i, Args&&... args) : Array<T>(a,Vector<d,int>(i,args...).product()), _size(Vector<d,int>(i,args...)) {}

      template <typename... Args> inline void resizeFast(int i, Args&&... args) {resizeFast(Vector<d,int>(i,args...));}
      inline void resizeFast(const Vector<d,int>& size) {
        _size = size;
        Array<T>::size(size.product());
      }

      template <typename... Args> inline void resize(int i, Args&&... args) {resize(Vector<d,int>(i,args...));}
      void resize(const Vector<d,int>& size) {
        int lines(Number::min(_size.product()/_size[0],size.product()/size[0]));
        Array<T>::size(size.product());
        if(size[0]<_size[0])  for(int i(0); i<lines; i++) memmove(&operator[](size[0]*i),&operator[](_size[0]*i),size[0]*4);
        else for(int i(lines-1); i>=0; i--) memmove(&operator[](size[0]*i),&operator[](_size[0]*i),_size[0]*4);
        _size = size;
      }
      int indexOf(const Vector<d,int>& point) const {
        int wtr(point[d-1]);
        for(int i(d-2); i>=0; i--) {
          wtr *= _size[i];
          wtr += point[i];
        }
        return wtr;
      }
      template <typename... Args> inline T& operator()(int i, Args&&... args) {return operator()(Vector<d,int>(i,args...));}
      template <typename... Args> inline const T& operator()(int i, Args&&... args) const {return operator()(Vector<d,int>(i,args...));}
      inline T& operator()(const Vector<d,int>& point) {return Array<T>::operator[](indexOf(point));}
      inline const T& operator()(const Vector<d,int>& point) const {return Array<T>::operator[](indexOf(point));}
      inline T& operator[](int i) {return Array<T>::operator[](i);}
      inline const T& operator[](int i) const {return Array<T>::operator[](i);}
      inline int size(int i) const {return _size[i];}
      inline int size() const {return _size.product();}
  };
}

#endif


