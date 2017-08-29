#include "Date.h"

using namespace L;

Stream& L::operator<<(Stream& s, const Date& v) {
  struct tm* tm;
  tm = localtime(&v._time);

  s << (tm->tm_year+1900);
  s << '-' << ntos(tm->tm_mon, 2);
  s << '-' << ntos(tm->tm_mday, 2);
  s << ' ' << ntos(tm->tm_hour, 2);
  s << ':' << ntos(tm->tm_min, 2);
  s << ':' << ntos(tm->tm_sec, 2);
  return s;
}
