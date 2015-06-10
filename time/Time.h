#ifndef DEF_L_Time
#define DEF_L_Time

#include "../stl/String.h"

namespace L {
  class Time {
    protected:
      long long usec;
    public:
      Time();
      Time(long long useconds);
      Time(long useconds, long mseconds, long seconds = 0, long minutes = 0, long hours = 0, long days = 0);

      Time operator+(const Time&) const;
      Time operator-(const Time&) const;
      Time operator*(float) const;
      Time operator*(const long long&) const;
      Time operator/(const long long&) const;
      bool operator==(const Time&) const;
      bool operator!=(const Time&) const;
      bool operator>(const Time&) const;
      bool operator<(const Time&) const;
      bool operator>=(const Time&) const;
      bool operator<=(const Time&) const;
      Time& operator+=(const Time&);
      Time& operator-=(const Time&);

      inline long days() const {return (long)(usec/86400000000LL);}
      inline long hours() const {return (long)(usec/3600000000LL);}
      inline long minutes() const {return (long)(usec/60000000LL);}
      inline long seconds() const {return (long)(usec/1000000LL);}
      inline long long milliseconds() const {return usec/1000LL;}
      inline long long microseconds() const {return usec;}

      float fSeconds() const;

      static Time now();
      static float fps();
      static String format(String, Time = now());
  };
}

#endif






