#pragma once

#include <ctime>
#include "../stream/Stream.h"

namespace L {
  class Date {
  protected:
    time_t _time;
  public:
    inline Date() = default;
    constexpr Date(time_t t) : _time(t) {}
    inline bool operator<(const Date& other) const { return difftime(_time, other._time) < 0.f; }

    inline uint32_t year() const { return localtime(&_time)->tm_year + 1900; }
    inline uint32_t month() const { return localtime(&_time)->tm_mon; }
    inline uint32_t day() const { return localtime(&_time)->tm_mday; }
    inline uint32_t hour() const { return localtime(&_time)->tm_hour; }
    inline uint32_t minute() const { return localtime(&_time)->tm_min; }
    inline uint32_t second() const { return localtime(&_time)->tm_sec; }
    inline time_t get_time() const { return _time; }

    inline static Date now() { return time(nullptr); }

    friend Stream& operator<<(Stream& s, const Date& v);
  };
  Stream& operator<<(Stream& s, const Date& v);
}
