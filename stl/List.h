#ifndef DEF_L_List
#define DEF_L_List

#include <cstdlib>
#include <list>
#include "../Serializable.h"

namespace L{
    template <class T>
    class List : public std::list<T>{
        public:
            List() : std::list<T>(){}
            List(const std::list<T>& v) : std::list<T>(v){}
            List(size_t n, const T& val = T()) : std::list<T>(n,val){}

            const T& operator[](size_t i) const{
                typename std::list<T>::const_iterator it(std::list<T>::begin());
                while(i--) it++;
                return *it;
            }
    };
}

#endif


