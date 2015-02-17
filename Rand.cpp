#include "Rand.h"

#include <ctime>
#include <iostream>

using namespace L;

//const ullong Rand::a(6364136223846793005LL), Rand::b(1442695040888963407LL);
const ullong Rand::a(2862933555777941757LL), Rand::b(3037000493LL);
ullong Rand::last(time(NULL));

ullong Rand::next(){
    return last = last*a+b;
}
byte Rand::nextByte(){
    return next()>>48;
}
double Rand::nextDouble(){
    return next()/(double)(ullong)-1;
}

ullong Rand::next(ullong min, ullong max){
    return (next()%(max-min+1))+min;
}
int Rand::next(int min, int max){
    return ((next()>>32)%(max-min+1))+min;
}
double Rand::next(double min, double max){
    return (nextDouble()*(max-min))+min;
}

double Rand::gauss(uint i){
    double wtr(0);
    for(uint j(0);j<i;j++)
        wtr += nextDouble();
    return wtr/i;
}
ullong Rand::next(ullong ave){
    if(ave){
        ullong wtr(0), thres(((ullong)-1)/(ave+1));
        while(next()>thres) wtr++;
        return wtr;
    }
    else return 0;
}
