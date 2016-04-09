#include "Rand.h"

#include <ctime>

using namespace L;

//const ullong Rand::a(6364136223846793005LL), Rand::b(1442695040888963407LL);
const ullong Rand::a(2862933555777941757LL), Rand::b(3037000493LL);
ullong Rand::last(time(nullptr));

ullong Rand::next() {
  return last = last*a+b;
}
int Rand::nextInt() {
  return next()>>32;
}
byte Rand::nextByte() {
  return next()>>48;
}
float Rand::nextFloat() {
  return next()/(float)(ullong)-1;
}

ullong Rand::next(ullong min, ullong max) {
  return (next()%(max-min+1))+min;
}
int Rand::next(int min, int max) {
  return ((next()>>32)%(max-min+1))+min;
}
float Rand::next(float min, float max) {
  return (nextFloat()*(max-min))+min;
}
void Rand::fill(byte* dst, size_t size) {
  while(size>0) {
    *dst++ = nextByte();
    size--;
  }
}

float Rand::gauss(uint i) {
  float wtr(0);
  for(uint j(0); j<i; j++)
    wtr += nextFloat();
  return wtr/i;
}
ullong Rand::next(ullong ave) {
  if(ave) {
    ullong wtr(0), thres(((ullong)-1)/(ave+1));
    while(next()>thres) wtr++;
    return wtr;
  } else return 0;
}
