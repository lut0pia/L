#pragma once

#include <cstdint>

namespace L {
  class Stream;
  class Time {
  protected:
    int64_t _us = 0;

  public:
    constexpr Time() {}
    constexpr Time(int64_t us)
        : _us(us) {}
    constexpr Time(long us, long ms, long s = 0, long m = 0, long h = 0, long d = 0)
        : _us(us + ms * 1000LL + s * 1000000LL + m * 60000000LL + h * 3600000000LL + d * 86400000000LL) {}

    constexpr Time operator+(const Time& other) const { return Time(_us + other._us); }
    constexpr Time operator-(const Time& other) const { return Time(_us - other._us); }
    constexpr Time operator-() const { return Time(-_us); }
    constexpr Time operator*(const Time& other) const { return Time(_us * other._us); }
    constexpr Time operator/(const Time& other) const { return Time(_us / other._us); }
    constexpr Time operator*(float other) const { return Time(int64_t(_us * other)); }
    constexpr Time operator*(int64_t other) const { return Time(_us * other); }
    constexpr Time operator/(int64_t other) const { return Time(_us / other); }
    constexpr Time& operator+=(const Time& other) {
      _us += other._us;
      return *this;
    }
    constexpr Time& operator-=(const Time& other) {
      _us -= other._us;
      return *this;
    }
    constexpr Time& operator*=(const Time& other) {
      _us *= other._us;
      return *this;
    }
    constexpr Time& operator/=(const Time& other) {
      _us /= other._us;
      return *this;
    }
    constexpr bool operator==(const Time& other) const { return _us == other._us; }
    constexpr bool operator!=(const Time& other) const { return _us != other._us; }
    constexpr bool operator>(const Time& other) const { return _us > other._us; }
    constexpr bool operator<(const Time& other) const { return _us < other._us; }
    constexpr bool operator>=(const Time& other) const { return _us >= other._us; }
    constexpr bool operator<=(const Time& other) const { return _us <= other._us; }

    constexpr int64_t days() const { return _us / 86400000000LL; }
    constexpr int64_t hours() const { return _us / 3600000000LL; }
    constexpr int64_t minutes() const { return _us / 60000000LL; }
    constexpr int64_t seconds() const { return _us / 1000000LL; }
    constexpr int64_t milliseconds() const { return _us / 1000LL; }
    constexpr int64_t microseconds() const { return _us; }

    constexpr float seconds_float() const { return float(seconds()) + (float(microseconds() % 1000000LL) / 1000000.f); }

    static Time now();

    friend Stream& operator<<(Stream& s, const Time& v);
  };

  Stream& operator<<(Stream& s, const Time& v);
}
