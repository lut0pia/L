#include "Time.h"

#include <windows.h>

using namespace L;

static struct WinTimeMult {
  double value;
  WinTimeMult() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    value = 1.0/(double(freq.QuadPart)/1000000.0);
  }
} win_time_mult;

Time Time::now() {
  int64_t counter;
  QueryPerformanceCounter((LARGE_INTEGER*)&counter);
  return Time(int64_t(counter * win_time_mult.value));
}
