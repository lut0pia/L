#ifndef DEF_L_Vector
#define DEF_L_Vector

#include <cstdarg>
#include <vector>

#include "../macros.h"
#include "../Rand.h"
#include "../Serializable.h"
#include "../stream.h"
#include "../tmp.h"

namespace L{
    template <class T>
    class Vector : public std::vector<T>, public Serializable{
        public:
            Vector() : std::vector<T>(){}
            Vector(const std::vector<T>& v) : std::vector<T>(v){}
            Vector(size_t n, const T& val = T()) : std::vector<T>(n,val){}
            Vector(T* a, size_t n) : std::vector<T>(n){
              for(uint i(0);i<n;i++)
                std::vector<T>::operator[](i) = a[i];
            }
            template <class InputIterator> Vector(InputIterator first, InputIterator last) : std::vector<T>(first,last){}

            bool operator==(const Vector& other) const{
                if(std::vector<T>::size() != other.size())
                    return false;
                for(uint i(0);i<std::vector<T>::size();i++)
                    if(std::vector<T>::operator[](i) != other[i])
                        return false;
                return true;
            }
            Vector& operator+=(const Vector& other){
                std::vector<T>::insert(std::vector<T>::end(),other.begin(),other.end());
                return (*this);
            }
            Vector& operator+=(const T& other){
                std::vector<T>::push_back(other);
                return (*this);
            }
            Vector operator+(const Vector& other){
                Vector wtr(*this);
                return wtr += other;
            }
            Vector operator+(const T& other){
                Vector wtr(*this);
                return wtr += other;
            }

            Vector reverse(){ // Returns a reversed std::vector
                Vector wtr;
                wtr.reserve(std::vector<T>::size());
                for(int i(std::vector<T>::size()-1);i>=0;i--)
                    wtr.push_back((*this)[i]);
                return wtr;
            }
            Vector sub(uint pos, uint size) const{
                return Vector(std::vector<T>::begin()+pos,std::vector<T>::begin()+pos+size);
            }
            const T& random() const{
                size_t i(Rand::next(0,std::vector<T>::size()-1));
                return std::vector<T>::operator[](i);
            }
            Vector shuffle(){ // Returns a shuffled std::vector
                Vector wtr(*this);
                shuffle(wtr);
                return wtr;
            }
            static void shuffle(Vector& v){ // Shuffles all elements in std::vector
                for(size_t i(0);i<v.size();i++)
                    Swap(v[i],v[Rn(0,v.size()-1)]);
            }
        private:
            void write(std::ostream& s) const{writeT<T>(s);}
            void read(std::istream& s){readT<T>(s);}

            template <typename R>
            typename enable_if<is_serializable<R> >::type writeT(std::ostream& s) const{
                s << "[ ";
                for(size_t i(0);i<std::vector<T>::size();i++){
                    Serializable::write(s,(*this)[i]);
                    s << " ";
                }
                s << "]";
            }

            template <typename R>
            typename enable_if<is_serializable<R> >::type readT(std::istream& s){
                nospace(s);
                if(s.peek()=='['){
                    std::vector<T>::clear();
                    s.ignore();
                    nospace(s);
                    while(s.peek()!=']'){
                        T tmp;
                        Serializable::read(s,tmp);
                        push_back(tmp);
                        nospace(s);
                    }
                    s.ignore();
                }
            }
            friend class Serializable;
    };
}

////////////////////////////////////////////////////////

namespace L{
    template <class T>
    inline std::vector<T> SubVector(const std::vector<T>& v, uint pos, uint size){ // Returns a subvector
        return std::vector<T>(v.begin()+pos,v.begin()+pos+size);
    }
}

#endif


