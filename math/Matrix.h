#ifndef DEF_L_Matrix
#define DEF_L_Matrix

#include "../geometry/Vector.h"

namespace L {
  template <int l, int c, class T>
  class Matrix;
  template <int s, class T>
  static T det(const Matrix<s,s,T>& m) {
    T wtr(0);
    for(int i(0); i<s; i++)
      wtr += ((i%2)?-1:1) * m(0,i) * det(m.sub(0,i));
    return wtr;
  }
  template <class T>
  static T det(const Matrix<2,2,T>& m) {
    return (m(0,0)*m(1,1))-(m(0,1)*m(1,0));
  }
  template <class T>
  static T det(const Matrix<1,1,T>& m) {
    return m(0,0);
  }
  template <int l, int c, class T>
  class Matrix {
    private:
      T _m[l][c];
    public:
      Matrix() {}
      Matrix(const Vector<l,T>& v) {
        for(int i(0); i<l; i++)
          _m[i][0] = v[i];
      }

      inline T& operator()(int line, int column) {return _m[line][column];}
      inline const T& operator()(int line, int column) const {return _m[line][column];}
      inline T* array() {return &_m[0][0];}
      inline const T* array() const {return &_m[0][0];}
      Matrix operator+(const Matrix& other) const {
        Matrix wtr(*this);
        wtr += other;
        return wtr;
      }
      Matrix operator-(const Matrix& other) const {
        Matrix wtr(*this);
        wtr -= other;
        return wtr;
      }
      template<int oc>
      Matrix<l,oc,T> operator*(const Matrix<c,oc,T>& other) const {
        Matrix<l,oc,T> wtr;
        for(int ci(0); ci<oc; ci++)
          for(int li(0); li<l; li++) {
            T s(0);
            for(int n(0); n<c; n++)
              s += _m[li][n]*other(n,ci);
            wtr(li,ci) = s;
          }
        return wtr;
      }
      Matrix<l,1,T> operator*(const Vector<l,T>& v) const {
        return (*this)*Matrix<l,1,T>(v);
      }
      Matrix& operator*=(const T& scalar) {
        for(int i(0); i<l*c; i++)
          array()[i] *= scalar;
        return *this;
      }
      Matrix& operator/=(const T& scalar) {
        for(int i(0); i<l*c; i++)
          array()[i] /= scalar;
        return *this;
      }
      Matrix operator*(const T& scalar) const {
        Matrix wtr(*this);
        wtr *= scalar;
        return wtr;
      }
      Matrix operator/(const T& scalar) const {
        Matrix wtr(*this);
        wtr /= scalar;
        return wtr;
      }
      Matrix<c,l,T> transpose() const {
        Matrix<c,l,T> wtr;
        for(int i(0); i<l; i++)
          for(int j(0); j<c; j++)
            wtr(i,j) = (*this)(j,i);
        return wtr;
      }
      Matrix<l-1,c-1,T> sub(int x, int y) const { // Returns matrix without column x and line y
        Matrix <l-1,c-1,T> wtr;
        int a,b,i,j;
        for(j=b=0; j<l; j++)
          if(j!=y) {
            for(i=a=0; i<c; i++)
              if(i!=x) {
                wtr(a,b) = (*this)(i,j);
                a++;
              }
            b++;
          }
        return wtr;
      }
      T det() const {
        return L::det(*this);
      }
      Matrix adjugate() const {
        Matrix wtr;
        for(int y(0); y<l; y++)
          for(int x(0); x<c; x++)
            wtr(x,y) = (((x+y)%2)?-1:1) * sub(x,y).det();
        return wtr;
      }
      inline Matrix inverse() const {
        return adjugate().transpose()/det();
      }

      static Matrix identity() {
        Matrix wtr;
        for(int x(0); x<c; x++)
          for(int y(0); y<l; y++)
            wtr(x,y) = ((x==y)?1:0);
        return wtr;
      }
      static Matrix<4,4,T> rotation(const Vector<3,T>& axis, T angle) {
        Matrix<4,4,T> wtr(Matrix<4,4,T>::identity());
        if(angle < 0.001 && angle > -0.001)
          angle = 0;
        T cosi(cos(angle));
        T sinu(sin(angle));
        const T& x(axis.x());
        const T& y(axis.y());
        const T& z(axis.z());
        T x2(axis.x()*axis.x());
        T y2(axis.y()*axis.y());
        T z2(axis.z()*axis.z());
        wtr(0,0) = x2+(cosi*(1-x2));
        wtr(0,1) = (x*y*(1-cosi))-(z*sinu);
        wtr(0,2) = (x*z*(1-cosi))+(y*sinu);
        wtr(1,0) = (x*y*(1-cosi))+(z*sinu);
        wtr(1,1) = y2+(cosi*(1-y2));
        wtr(1,2) = (y*z*(1-cosi))-(x*sinu);
        wtr(2,0) = (x*z*(1-cosi))-(y*sinu);
        wtr(2,1) = (y*z*(1-cosi))+(x*sinu);
        wtr(2,2) = z2+(cosi*(1-z2));
        return wtr;
      }
      static Matrix<4,4,T> translation(const Vector<3,T>& vector) {
        Matrix<4,4,T> wtr(Matrix<4,4,T>::identity());
        wtr(0,3) = vector.x();
        wtr(1,3) = vector.y();
        wtr(2,3) = vector.z();
        return wtr;
      }
      static Matrix<4,4,T> orientation(const Vector<3,T>& newx, const Vector<3,T>& newy,const Vector<3,T>& newz) {
        Matrix<4,4,T> wtr(Matrix<4,4,T>::identity());
        wtr(0,0) = newx.x();
        wtr(1,0) = newx.y();
        wtr(2,0) = newx.z();
        wtr(0,1) = newy.x();
        wtr(1,1) = newy.y();
        wtr(2,1) = newy.z();
        wtr(0,2) = newz.x();
        wtr(1,2) = newz.y();
        wtr(2,2) = newz.z();
        return wtr;
      }

      operator Vector<l,T>() {
        Vector<l,T> wtr;
        for(int i(0); i<l; i++)
          wtr[i] = _m[i][0];
        return wtr;
      }
  };

  typedef Matrix<3,3,float> Matrix33f;
  typedef Matrix<4,4,float> Matrix44f;

  template <int l, int c, class T>
  Stream& operator<<(Stream &s, const Matrix<l,c,T>& m) {
    for(int li(0); li<l; li++) {
      for(int ci(0); ci<c; ci++)
        s << '[' << m(li,ci) << "]\t";
      s << '\n';
    }
    return s;
  }
}

#endif





