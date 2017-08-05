#pragma once

namespace L {
  template <class T> inline const T& PI() {
    static const T wtr((T)3.14159265358979323846);
    return wtr;
  }
  template <class T> inline const T& E() {
    static const T wtr((T)2.71828182845904523536);
    return wtr;
  }
  template <class T> inline const T& DEGTORAD() {
    static const T wtr(PI<T>()/(T)180);
    return wtr;
  }
  template <class T> inline const T& RADTODEG() {
    static const T wtr(1.f/DEGTORAD<T>());
    return wtr;
  }
}