#ifndef DEF_L_general
#define DEF_L_general

#include <iostream>
#include <cstring>
#include "macros.h"

namespace L{
    // Number functions :
    unsigned long factorial(unsigned long n);
    uint digit(uint d, uint b, uint n);

    // Template functions :
    template <class T>
    bool InBetween(const T& n, const T& a, const T& b){
        return (a <= n && n <= b) || (a >= n && n >= b);
    }
    template <class T>
    T PMod(T n, T limit){
        while(n>=limit)n-=limit;
        while(n<0)n+=limit;
        return n;
    }
    template <class T>
    T PModDiff(T n1, T n2, T limit){
        n1 -= n2;
        while(n1>limit/2)n1-=limit;
        while(n1<-limit/2)n1+=limit;
        return n1;
    }
    template <class T>
    void swap(T& a, T& b){
        char tmp[sizeof(T)];
        memcpy(tmp,&a,sizeof(T));
        memcpy(&a,&b,sizeof(T));
        memcpy(&b,tmp,sizeof(T));
    }
}

#endif

