#ifndef DEF_L_MultiArray
#define DEF_L_MultiArray

#include "Array.h"
#include "../geometry/Vector.h"
#include "../math/math.h"

inline void memswap(void* a, void* b, size_t size) {
  if(a==b) return;
  char buffer[16384];
  while(size>0) {
    size_t bufsize(L::min(L::min(size,(size_t)L::abs((int)a-(int)b)),16384u));
    memcpy(buffer,a,bufsize);
    memcpy(a,b,bufsize);
    memcpy(b,buffer,bufsize);
    a = (char*)a+bufsize;
    b = (char*)b+bufsize;
    size -= bufsize;
  }
}

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
        Array<T>::size(size.product());
        if(size[0]<_size[0]) {
          int lines(_size.product()/_size[0]);
          for(int i(0); i<lines; i++)
            memswap(&operator[](size[0]*i),&operator[](_size[0]*i),size[0]*sizeof(T));
        } else
          for(int i((size.product()/size[0])-1); i>=0; i--)
            memswap(&operator[](size[0]*i),&operator[](_size[0]*i),_size[0]*sizeof(T));
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


