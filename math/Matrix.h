#ifndef DEF_L_Matrix
#define DEF_L_Matrix

#include "../geometry/Point.h"

namespace L {
  template <int l, int c, class T>
  class Matrix {
    private:
      union {
        T _m[l][c];
        T _a[l*c];
      };
    public:
      Matrix() {}
      Matrix(const Point<l,T>& v) {
        for(size_t i(0); i<l; i++)
          _m[i][0] = v[i];
      }
      inline T& operator()(int line, int column) {return _m[line][column];}
      inline const T& operator()(int line, int column) const {return _m[line][column];}
      Matrix operator+(const Matrix<l,c,T>& other) const {
        Matrix wtr(*this);
        wtr += other;
        return wtr;
      }
      Matrix operator-(const Matrix<l,c,T>& other) const {
        Matrix wtr(*this);
        wtr -= other;
        return wtr;
      }
      template<int oc>
      Matrix<l,oc,T> operator*(const Matrix<c,oc,T>& other) const {
        Matrix<l,oc,T> wtr;
        for(size_t x(0); x<oc; x++)
          for(size_t y(0); y<l; y++) {
            T s(0);
            for(size_t n(0); n<c; n++)
              s += (*this)(n,y)*other(x,n);
            wtr(x,y) = s;
          }
        return wtr;
      }
      Matrix<l,1,T> operator*(const Point<l,T>& v) const {
        return (*this)*Matrix<l,1,T>(v);
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
      Matrix<c,l,T> trans() const {
        Matrix<c,l,T> wtr;
        for(size_t i(0); i<l; i++)
          for(size_t j(0); j<c; j++)
            wtr(i,j) = (*this)(j,i);
        return wtr;
      }
      Matrix<l-1,c-1,T> sub(size_t x, size_t y) const { // Returns matrix without x column and y line
        Matrix <l-1,c-1,T> wtr;
        size_t a,b,i,j;
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
        if(l!=c) throw Exception("Only square matrices have a determinant.");
        if(c==1)
          return (*this)(0,0);
        else if(c==2)
          return ((*this)(0,0)*(*this)(1,1))-((*this)(0,1)*(*this)(1,0));
        else {
          T wtr(0);
          for(size_t i(0); i<c; i++)
            wtr += (i%2)?-1:1 * (*this)(i,0) * sub(i,0).det();
          return wtr;
        }
      }
      Matrix adjugate() const {
        Matrix wtr;
        for(size_t y(0); y<l; y++)
          for(size_t x(0); x<c; x++)
            wtr(x,y) = ((x+y)%2)?-1:1 * sub(x,y).det();
        return wtr;
      }
      operator Point<2,T>() {
        return Point<2,T>((*this)(0,0),(*this)(0,1));
      }
      operator Point<3,T>() {
        return Point<3,T>((*this)(0,0),(*this)(0,1),(*this)(0,2));
      }

      static Matrix identity() {
        Matrix wtr;
        for(size_t x(0); x<c; x++)
          for(size_t y(0); y<l; y++)
            wtr(x,y) = ((x==y)?1:0);
        return wtr;
      }
  };

  template <int l, int c, class T>
  std::ostream& operator<<(std::ostream &stream, const Matrix<l,c,T>& m) {
    for(size_t y(0); y<l; y++) {
      for(size_t x(0); x<c; x++)
        stream << '[' << m(x,y) << "]\t";
      stream << std::endl;
    }
    return stream;
  }

  typedef Matrix<3,3,float> Matrix33f;
  typedef Matrix<4,4,float> Matrix44f;
}

#endif





