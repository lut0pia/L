#pragma once

#include <cstdint> 

namespace L {
  class String;
  class Stream;
  class Time {
  protected:
    int64_t usec;
  public:
    inline Time() = default;
    constexpr Time(int64_t us) : usec(us) {}
    constexpr Time(long us,long ms,long s = 0,long m = 0,long h = 0,long d = 0)
      : usec(us+ms*1000LL+s*1000000LL+m*60000000LL+h*3600000000LL+d*86400000000LL) {}

    inline Time operator+(const Time& other) const{ return Time(usec+other.usec); }
    inline Time operator-(const Time& other) const{ return Time(usec-other.usec); }
    inline Time operator*(const Time& other) const{ return Time(usec*other.usec); }
    inline Time operator/(const Time& other) const{ return Time(usec/other.usec); }
    inline Time operator*(float other) const{ return Time(usec*other); }
    inline Time operator*(int64_t other) const{ return Time(usec*other); }
    inline Time operator/(int64_t other) const{ return Time(usec/other); }
    inline Time& operator+=(const Time& other){ usec += other.usec; return *this; }
    inline Time& operator-=(const Time& other){ usec -= other.usec; return *this; }
    inline Time& operator*=(const Time& other){ usec *= other.usec; return *this; }
    inline Time& operator/=(const Time& other){ usec /= other.usec; return *this; }
    inline bool operator==(const Time& other) const { return usec == other.usec; }
    inline bool operator!=(const Time& other) const { return usec != other.usec; }
    inline bool operator>(const Time& other) const { return usec > other.usec; }
    inline bool operator<(const Time& other) const { return usec < other.usec; }
    inline bool operator>=(const Time& other) const { return usec >= other.usec; }
    inline bool operator<=(const Time& other) const { return usec <= other.usec; }

    inline long days() const { return (long)(usec/86400000000LL); }
    inline long hours() const { return (long)(usec/3600000000LL); }
    inline long minutes() const { return (long)(usec/60000000LL); }
    inline long seconds() const { return (long)(usec/1000000LL); }
    inline long long milliseconds() const { return usec/1000LL; }
    inline long long microseconds() const { return usec; }

    inline float fSeconds() const { return float(seconds())+(float(microseconds()%1000000LL)/1000000.f); }

    static Time now();

    friend Stream& operator<<(Stream &s, const Time& v);
  };
}






