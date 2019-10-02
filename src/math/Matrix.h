#pragma once

#include "Vector.h"

namespace L {
  template <int l,int c,class T> class Matrix;
  template <int s,class T>
  static T det(const Matrix<s,s,T>& m) {
    T wtr(0);
    for(int i(0); i<s; i++)
      wtr += ((i%2) ? -m(0,i) : m(0,i)) * det(m.sub(0,i));
    return wtr;
  }
  template <class T> inline static T det(const Matrix<2,2,T>& m) { return (m(0,0)*m(1,1))-(m(0,1)*m(1,0)); }
  template <class T> inline static T det(const Matrix<1,1,T>& m) { return m(0,0); }
  template <int l,int c,class T>
  class Matrix {
  private:
    T _m[c][l];
  public:
    inline Matrix() = default;
    Matrix(const T& s){
      for(int i(0); i<c; i++)
        for(int j(0); j<l; j++)
          _m[i][j] = (i==j) ? s : 0;
    }
    Matrix(const std::initializer_list<T>& il) {
      uintptr_t i(0);
      for(const T& s : il) {
        _m[0][i++] = s;
      }
    }
    template<int ol, int oc>
    Matrix(const Matrix<ol, oc, T>& other) {
      static_assert(l<ol && c<oc, "Cannot initialize matrix from smaller matrix");
      for(int i(0); i<c; i++)
        for(int j(0); j<l; j++)
          _m[i][j] = other._m[i][j];
    }

    inline T& operator()(uintptr_t line, uintptr_t column) { return _m[column][line]; }
    inline const T& operator()(uintptr_t line, uintptr_t column) const { return _m[column][line]; }
    inline T* array() { return &_m[0][0]; }
    inline const T* array() const { return &_m[0][0]; }
    template <int size = l> inline Vector<size, T>& vector(uintptr_t i) { return *(Vector<size, T>*)(((Vector<l, T>*)_m)+i); }
    template <int size = l> inline const Vector<size, T>& vector(uintptr_t i) const { return *(const Vector<size, T>*)(((const Vector<l, T>*)_m)+i); }
    template <int size = c> inline Vector<size, T> row(uintptr_t i) const {
      Vector<size, T> wtr;
      for(uintptr_t j(0); j<size; j++)
        wtr[j] = _m[j][i];
      return wtr;
    }

    inline Matrix& operator+=(const Matrix& other) {
      for(int i(0); i<l*c; i++)
        array()[i] += other.array()[i];
      return *this;
    }
    inline Matrix& operator-=(const Matrix& other) {
      for(int i(0); i<l*c; i++)
        array()[i] -= other.array()[i];
      return *this;
    }
    inline Matrix operator+(const Matrix& other) const { return Matrix(*this) += other; }
    inline Matrix operator-(const Matrix& other) const { return Matrix(*this) -= other; }
    template<int oc>
    Matrix<l,oc,T> operator*(const Matrix<c,oc,T>& other) const {
      Matrix<l,oc,T> wtr;
      for(int ci(0); ci<oc; ci++)
        for(int li(0); li<l; li++) {
          T s(0);
          for(int n(0); n<c; n++)
            s += operator()(li,n)*other(n,ci);
          wtr(li,ci) = s;
        }
      return wtr;
    }
    inline Matrix<l,1,T> operator*(const Vector<l,T>& v) const {
      return (*this)*(*(const Matrix<l,1,T>*)&v);
    }
    inline Matrix& operator*=(const T& scalar) {
      for(int i(0); i<l*c; i++)
        array()[i] *= scalar;
      return *this;
    }
    inline Matrix& operator/=(const T& scalar) {
      for(int i(0); i<l*c; i++)
        array()[i] /= scalar;
      return *this;
    }
    inline Matrix operator*(const T& scalar) const { return Matrix(*this) *= scalar; }
    inline Matrix operator/(const T& scalar) const { return Matrix(*this) /= scalar; }
    Matrix<c,l,T> transpose() const {
      Matrix<c,l,T> wtr;
      for(int i(0); i<l; i++)
        for(int j(0); j<c; j++)
          wtr(i,j) = (*this)(j,i);
      return wtr;
    }
    Matrix<l-1,c-1,T> sub(int x,int y) const { // Returns matrix without column x and line y
      Matrix<l - 1, c - 1, T> wtr {};
      int a,b,i,j;
      for(j = b = 0; j<l; j++)
        if(j!=y) {
          for(i = a = 0; i<c; i++)
            if(i!=x) {
              wtr(a,b) = (*this)(i,j);
              a++;
            }
          b++;
        }
      return wtr;
    }
    inline T det() const { return L::det(*this); }
    Matrix adjugate() const {
      Matrix wtr;
      for(int y(0); y<l; y++)
        for(int x(0); x<c; x++)
          wtr(x,y) = (((x+y)%2) ? -sub(x,y).det() : sub(x,y).det());
      return wtr;
    }
    inline Matrix inverse() const { return adjugate().transpose()/det(); }

    inline operator Vector<l,T>() { return *(const Vector<l,T>*)this; }

    friend Stream& operator<<(Stream &s, const Matrix& m) {
      for(int li(0); li<l; li++) {
        for(int ci(0); ci<c; ci++)
          s << '[' << m(li, ci) << "]\t";
        s << '\n';
      }
      return s;
    }
    friend Stream& operator<(Stream& s, const Matrix& m) {
      for(uintptr_t i(0); i<l*c; i++)
        s < m.array()[i];
      return s;
    }
    friend Stream& operator>(Stream& s, Matrix& m) {
      for(uintptr_t i(0); i<l*c; i++)
        s > m.array()[i];
      return s;
    }
    template <int ol, int oc, class R> friend class Matrix;
  };

  typedef Matrix<3,3,float> Matrix33f;
  typedef Matrix<4,4,float> Matrix44f;
}
