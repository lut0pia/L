#ifndef DEF_L_MultiArray
#define DEF_L_MultiArray

#include <cstdarg>
#include <cstring>
#include "../geometry/Point.h"
#include "../macros.h"
#include "Array.h"

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
  class MultiArray : private Array<T> {
    protected:
      Point<d,int> _size;
    public:
      MultiArray() : Array<T>(), _size(0) {}
      MultiArray(int arg,...) : Array<T>() {
        ARGSTOPOINT
        resizeFast(point);
      }
      void resizeFast(int arg,...) {
        ARGSTOPOINT
        resizeFast(point);
      }
      void resizeFast(const Point<d,int>& size) {
        _size = size;
        Array<T>::size(size.product());
      }

      void resize(int arg,...) {
        ARGSTOPOINT
        resize(point);
      }
      void resize(const Point<d,int>& size) {
        MultiArray<d,T> old(*this);
        _size = size;
        int pos[d] = {0}, oldpos, newpos, zeroCount;
        Array<T>::clear();
        Array<T>::size(size.product());
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
      int indexOf(const Point<d,int>& point) const {
        int wtr(point[0]);
        for(int i(1); i<d; i++) {
          wtr *= _size[i];
          wtr += point[i];
        }
        return wtr;
      }
      T& operator()(int pos,...) {ACCESS}
      const T& operator()(int pos,...) const {ACCESS}
      T& operator()(const Point<d,int>& point) {return Array<T>::operator[](indexOf(point));}
      const T& operator()(const Point<d,int>& point) const {return Array<T>::operator[](indexOf(point));}
      inline T& operator[](int i) {return Array<T>::operator[](i);}
      inline const T& operator[](int i) const {return Array<T>::operator[](i);}
      inline int size(int i) const {return _size[i];}
      inline int size() const {return _size.product();}
  };
}
#undef ARGSTOPOINT
#undef ACCESS

#endif


