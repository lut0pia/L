#ifndef DEF_L_Matrix
#define DEF_L_Matrix

#include "../containers/Array.h"
#include "../geometry.h"

namespace L{
    template <class T>
    class Matrix : public Array<2,T>{
        public:
            Matrix(){}
            Matrix(size_t width, size_t height) : Array<2,T>(width,height){}
            Matrix(const Point<3,T>& p) : Array<2,T>(1,3){
                (*this)(0,0) = p.x();
                (*this)(0,1) = p.y();
                (*this)(0,2) = p.z();
            }

            inline size_t width() const{return Array<2,T>::size(0);}
            inline size_t height() const{return Array<2,T>::size(1);}

            Matrix operator+(const Matrix<T>& b) const{
                if(width()==b.width() && height()==b.height()){
                    Matrix wtr(*this);
                    for(size_t i=0;i<Array<2,T>::data.size();i++)
                        wtr.data[i] += b.data[i];
                    return wtr;
                }
                return Matrix<T>();
            }
            Matrix operator-(const Matrix<T>& b) const{
                if(width()==b.width() && height()==b.height()){
                    Matrix wtr(*this);
                    for(size_t i=0;i<wtr.data.size();i++)
                        wtr.data[i] -= b.data[i];
                    return wtr;
                }
                return Matrix<T>();
            }
            Matrix operator*(const Matrix<T>& b) const{
                Matrix wtr(b.width(),height());
                T s;

                if(width()==b.height()){
                    for(size_t x=0;x<b.width();x++)
                        for(size_t y=0;y<height();y++){
                            s = 0;
                            for(size_t n=0;n<width();n++)
                                s += (*this)(n,y)*b(x,n);
                            wtr(x,y) = s;
                        }
                }
                return wtr;
            }
            Matrix operator*(const T& b) const{
                Matrix wtr(*this);
                for(size_t i=0;i<wtr.data.size();i++)
                    wtr.data[i] *= b;
                return wtr;
            }
            Matrix operator/(const T& b) const{
                Matrix wtr(*this);
                for(size_t i=0;i<wtr.data.size();i++)
                    wtr.data[i] /= b;
                return wtr;
            }
            Matrix<T> trans() const{
                Matrix<T> wtr(height(),width());
                for(size_t i=0;i<height();i++)
                    for(size_t j=0;j<width();j++)
                        wtr(i,j) = (*this)(j,i);
                return wtr;
            }
            Matrix<T> sub(size_t x, size_t y) const{ // Returns matrix without x column and y line
                Matrix <T> wtr(width()-1,height()-1);
                size_t a,b,i,j;
                for(j=b=0;j<height();j++)
                    if(j!=y){
                        for(i=a=0;i<width();i++)
                            if(i!=x){
                                wtr(a,b) = (*this)(i,j);
                                a++;
                            }
                        b++;
                    }
                return wtr;
            }
            T det() const{
                if(height()==width() && width()){
                    if(width()==1)
                        return (*this)(0,0);
                    else if(width()==2)
                        return ((*this)(0,0)*(*this)(1,1))-((*this)(0,1)*(*this)(1,0));
                    else{
                        T wtr(0);
                        for(size_t i=0;i<width();i++)
                            wtr += (i%2)?-1:1 * (*this)(i,0) * sub(i,0).det();
                        return wtr;
                    }
                }
            }
            Matrix<T> adjugate() const{
                Matrix <T> wtr(width(),height());
                for(size_t y=0;y<height();y++)
                    for(size_t x=0;x<width;x++)
                        wtr(x,y) = ((x+y)%2)?-1:1 * sub(x,y).det();
                return wtr;
            }
            operator Point<2,T>(){
                return Point<2,T>((*this)(0,0),(*this)(0,1));
            }
            operator Point<3,T>(){
                return Point<3,T>((*this)(0,0),(*this)(0,1),(*this)(0,2));
            }

            static Matrix<T> identity(size_t n){
                Matrix<T> wtr(n,n);
                for(size_t x=0;x<n;x++)
                    for(size_t y=0;y<n;y++)
                        wtr(x,y) = ((x==y)?1:0);
                return wtr;
            }
    };
    template <class T>
    std::ostream& operator<<(std::ostream &stream, const Matrix<T>& M){
        for(size_t y=0;y<M.height();y++){
            for(size_t x=0;x<M.width();x++)
                stream << '[' << M(x,y) << "]\t";
            stream << std::endl;
        }
        return stream;
    }
}

#endif





