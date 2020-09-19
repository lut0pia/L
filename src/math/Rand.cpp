#include "Rand.h"

#include <ctime>

using namespace L;

//const ullong Rand::a(6364136223846793005LL), Rand::b(1442695040888963407LL);
const uint64_t Rand::a(2862933555777941757LL), Rand::b(3037000493LL);
uint64_t Rand::last(time(nullptr));

uint64_t Rand::next() {
  return last = last*a+b;
}
int Rand::next_int() {
  return next()>>32;
}
uint8_t Rand::next_byte() {
  return uint8_t(next()>>48);
}
float Rand::next_float() {
  return next()/(float)(uint64_t)-1;
}

uint64_t Rand::next(uint64_t min,uint64_t max) {
  return (next()%(max-min+1))+min;
}
int Rand::next(int min, int max) {
  return ((next()>>32)%(max-min+1))+min;
}
float Rand::next(float min, float max) {
  return (next_float()*(max-min))+min;
}
void Rand::fill(uint8_t* dst, size_t size) {
  while(size>0) {
    *dst++ = next_byte();
    size--;
  }
}

float Rand::gauss(uint32_t i) {
  float wtr(0);
  for(uint32_t j(0); j<i; j++)
    wtr += next_float();
  return wtr/i;
}
uint64_t Rand::next(uint64_t ave) {
  if(ave) {
    uint64_t wtr(0), thres(((uint64_t)-1)/(ave+1));
    while(next()>thres) wtr++;
    return wtr;
  } else return 0;
}
