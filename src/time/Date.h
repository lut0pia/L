#pragma once

#include <ctime>
#include "../stream/Stream.h"

namespace L {
  class Date {
  protected:
    time_t _time;
  public:
    inline Date() = default;
    inline Date(time_t t) : _time(t) {}
    inline bool operator<(const Date& other) const { return difftime(_time, other._time)<0.f; }

    inline static Date now() { return time(nullptr); }

    friend Stream& operator<<(Stream& s, const Date& v);
  };
}
