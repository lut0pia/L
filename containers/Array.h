#ifndef DEF_L_Array
#define DEF_L_Array

#include <cstdarg>
#include <cstring>
#include "../geometry/Point.h"
#include "../stl/Vector.h"
#include "../macros.h"

#define ARGSTOPOINT Point<d,int> point;\
                    if(1){\
                    int i(0);\
                    point[0] = arg;\
                    va_list vl;\
                    va_start(vl,arg);\
                    while(++i<d) point[i] = va_arg(vl,int);\
                    va_end(vl);}
#define ACCESS  int i(0);\
                va_list vl;\
                va_start(vl,pos);\
                while(++i<d){\
                  pos *= _size[i];\
                  pos += va_arg(vl,int);\
                }\
                va_end(vl);\
                return (*this)[pos];
namespace L {
  template <int d,class T>
  class Array : private Vector<T> {
    protected:
      Point<d,int> _size;
    public:
      Array() : Vector<T>(), _size(0) {}
      Array(int arg,...) : Vector<T>() {
        ARGSTOPOINT
        resizeFast(point);
      }
      void resizeFast(int arg,...) {
        ARGSTOPOINT
        resizeFast(point);
      }
      void resizeFast(const Point<d,int>& size) {
        _size = size;
        Vector<T>::resize(size.product());
      }
      /*
      void resize(size_t newsize,...){
          size_t oldSizes[d];
          memcpy(oldSizes,sizes,d*sizeof(size_t));

          RESIZE

          Vector<T>::reserve(newsize);
          for(int i=d-1;i>=0;i--){ // For every dimension starting from last
              typename Vector<T>::iterator it(Vector<T>::end());
              size_t rowSize(1);
              for(int j=0;j<i;j++)
                  rowSize *= oldSizes[j];

              while(it!=Vector<T>::begin()){
                  if(sizes[i]>=oldSizes[i])
                      Vector<T>::insert(it,(sizes[i]-oldSizes[i])*(rowSize/oldSizes[i]),T());
                  else
                      Vector<T>::erase(it+(sizes[i]-oldSizes[i])*(rowSize/oldSizes[i]),it);
                  it += (sizes[i]-oldSizes[i])*(rowSize/oldSizes[i]);
                  it -= rowSize;
              }
          }
      }
      */

      void resize(int arg,...) {
        ARGSTOPOINT
        resize(point);
      }
      void resize(const Point<d,int>& size) {
        Array<d,T> old(*this);
        _size = size;
        int pos[d] = {0}, oldpos, newpos, zeroCount;
        Vector<T>::clear();
        Vector<T>::resize(size.product());
        // Restore old elements
        for(int i(0); i<d; i++)
          if(!old.size(i))
            return;
        do {
          // Replace element
          oldpos = newpos = pos[0];
          for(int i(1); i<d; i++) {
            oldpos *= old.size(i);
            newpos *= _size[i];
            oldpos += pos[i];
            newpos += pos[i];
          }
          (*this)[newpos] = old[oldpos];
          // Change position
          zeroCount = 0;
          for(int i(0); i<d; i++) {
            pos[i]++;
            if(pos[i]<std::min(_size[i],old.size(i))) break;
            else {
              pos[i] = 0;
              zeroCount++;
            }
          }
        } while(zeroCount<d); // All positions have been done
      }
      int indexOf(const Point<d,int>& point) const{
        int wtr(point[0]);
        for(int i(1);i<d;i++){
          wtr *= _size[i];
          wtr += point[i];
        }
        return wtr;
      }
      T& operator()(int pos,...) {ACCESS}
      const T& operator()(int pos,...) const {ACCESS}
      T& operator()(const Point<d,int>& point){return Vector<T>::operator[](indexOf(point));}
      const T& operator()(const Point<d,int>& point) const{return Vector<T>::operator[](indexOf(point));}
      inline T& operator[](int i) {return Vector<T>::operator[](i);}
      inline const T& operator[](int i) const {return Vector<T>::operator[](i);}
      inline int size(int i) const {return _size[i];}
      inline int size() const {return _size.product();}
  };
}
#undef ARGSTOPOINT
#undef ACCESS

#endif


