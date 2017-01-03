#include "Time.h"

#if defined L_WINDOWS
#include <windows.h>
#elif defined L_UNIX
#include <sys/time.h>
#endif

#include "Timer.h"
#include "../text/String.h"

using namespace L;

Time Time::now() {
#if defined L_WINDOWS
  int64_t counter,freq;
  QueryPerformanceCounter((LARGE_INTEGER*)&counter);
  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
  freq /= 1000000LL;
  return Time(counter / freq);
#elif defined L_UNIX
  struct timeval tv;
  gettimeofday(&tv,nullptr);
  return Time(tv.tv_usec,0,tv.tv_sec);
#endif
}

Stream& L::operator<<(Stream &s,const Time& v) {
  int c(-1);
  int64_t us(v.microseconds());
  if(us<0) {
    us = -us;
    s << '-';
  }
  long long days(us/86400000000LL);
  if(days) { s << days << 'd'; c++; }
  long long hours((us/3600000000LL)%24LL);
  if(hours) { s << ntos<10>(hours,2) << 'h'; if(++c) return s; }
  long long mins((us/60000000LL)%60LL);
  if(mins) { s << ntos<10>(mins,2) << 'm'; if(++c) return s; }
  long long secs((us/1000000LL)%60LL);
  if(secs) { s << ntos<10>(secs,2) << 's'; if(++c) return s; }
  long long msecs((us/1000LL)%1000LL);
  if(msecs) { s << ntos<10>(msecs,3) << "ms"; if(++c) return s; }
  long long usecs(us%1000LL);
  if(usecs) { s << ntos<10>(usecs,3) << "us"; if(++c) return s; }
  return s;
}
