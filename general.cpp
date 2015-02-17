#include "general.h"

#include <cmath>
#include "Exception.h"

using namespace L;

unsigned long L::factorial(unsigned long n){
    unsigned long wtr=1;
    while(n)wtr*=n--;
    return wtr;
}
uint L::digit(uint d, uint b, uint n){
     return n/((int)pow(b,d))%b;
}


