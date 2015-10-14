#include "Time.h"

#if defined L_WINDOWS
#include <windows.h>
#include <sys/timeb.h>
#elif defined L_UNIX
#include <sys/time.h>
#endif

#include <ctime>
#include "../systems.h"
#include "Timer.h"
#include "../types.h"

using namespace L;

Time::Time() {}
Time::Time(long long us) : usec(us) {}
Time::Time(long us, long ms, long s, long m, long h, long d)
  : usec(us+ms*1000LL+s*1000000LL+m*60000000LL+h*3600000000LL+d*86400000000LL) {}

Time Time::operator+(const Time& other) const {
  return Time(usec+other.usec);
}
Time Time::operator-(const Time& other) const {
  return Time(usec-other.usec);
}
Time Time::operator*(float v) const {
  return Time(usec*v);
}
Time Time::operator*(const long long& v) const {
  return Time(usec*v);
}
Time Time::operator/(const long long& v) const {
  return Time(usec/v);
}
bool Time::operator==(const Time& other) const {
  return usec == other.usec;
}
bool Time::operator!=(const Time& other) const {
  return usec != other.usec;
}
bool Time::operator>(const Time& other) const {
  return usec > other.usec;
}
bool Time::operator<(const Time& other) const {
  return usec < other.usec;
}
bool Time::operator>=(const Time& other) const {
  return usec >= other.usec;
}
bool Time::operator<=(const Time& other) const {
  return usec <= other.usec;
}
Time& Time::operator+=(const Time& other) {
  usec += other.usec;
  return *this;
}
Time& Time::operator-=(const Time& other) {
  usec -= other.usec;
  return *this;
}

float Time::fSeconds() const {
  return ((float)seconds())+((float)(microseconds()%1000000LL)/1000000.f);
}

Time Time::now() {
#if defined L_WINDOWS
  union {
    FILETIME ft;
    ullong nano;
  };
  GetSystemTimeAsFileTime(&ft);
  return Time(nano/10LL);
#elif defined L_UNIX
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return Time(tv.tv_usec,0,tv.tv_sec);
#endif
}
Timer fpsTimer;
float Time::fps() {
  return 1.f/fpsTimer.frame().fSeconds();
}
String Time::format(String str, Time t) {
  time_t timestamp(t.seconds());
  char date[512];
  strftime(date,sizeof(date),str,localtime(&timestamp));
  return date;
}
