#include "Rand.h"

#include <ctime>

using namespace L;

constexpr uint64_t a = 2862933555777941757LL, b = 3037000493LL;
static uint64_t last = time(nullptr);

uint64_t Rand::next() {
  return last = last*a+b;
}
uint8_t Rand::next_byte() {
  return uint8_t(next()>>48);
}
float Rand::next_float() {
  return next() / float(UINT64_MAX);
}

uint64_t Rand::next(uint64_t max) {
  return next() % max;
}
uint64_t Rand::next(uint64_t min, uint64_t max) {
  return (next(max - min + 1)) + min;
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
uint64_t Rand::next_exp(uint64_t ave) {
  if(ave) {
    uint64_t wtr = 0;
    uint64_t thres = UINT64_MAX / (ave + 1);
    while(next() > thres) {
      wtr++;
    }
    return wtr;
  } else {
    return 0;
  }
}

uint64_t Rand::get_state() {
  return last;
}
void Rand::set_state(uint64_t s) {
  last = s;
}