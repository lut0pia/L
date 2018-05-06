#include "Time.h"

#include <sys/time.h>

using namespace L;

Time Time::now() {
  struct timeval tv;
  gettimeofday(&tv,nullptr);
  return Time(tv.tv_usec,0,tv.tv_sec);
}
