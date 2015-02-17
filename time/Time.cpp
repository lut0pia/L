#include "Time.h"

#include <ctime>
#include "../macros.h"
#if defined L_WINDOWS
    #include <sys/timeb.h>
#elif defined L_UNIX
    #include <sys/time.h>
#endif

using namespace L;

Time::Time(){}
Time::Time(long long us) : usec(us){}
Time::Time(long us, long ms, long s, long m, long h, long d)
: usec(us+ms*1000LL+s*1000000LL+m*60000000LL+h*3600000000LL+d*86400000000LL){}

Time Time::operator+(const Time& other) const{
    return Time(usec+other.usec);
}
Time Time::operator-(const Time& other) const{
    return Time(usec-other.usec);
}
Time Time::operator*(const long long& v) const{
    return Time(usec*v);
}
Time Time::operator/(const long long& v) const{
    return Time(usec/v);
}
bool Time::operator==(const Time& other) const{
    return usec == other.usec;
}
bool Time::operator!=(const Time& other) const{
    return usec != other.usec;
}
bool Time::operator>(const Time& other) const{
    return usec > other.usec;
}
bool Time::operator<(const Time& other) const{
    return usec < other.usec;
}
bool Time::operator>=(const Time& other) const{
    return usec >= other.usec;
}
bool Time::operator<=(const Time& other) const{
    return usec <= other.usec;
}
Time& Time::operator+=(const Time& other){
    usec += other.usec;
    return *this;
}
Time& Time::operator-=(const Time& other){
    usec -= other.usec;
    return *this;
}

double Time::fSeconds() const{
    return ((double)seconds())+((double)(microseconds()%1000000LL)/1000000.0);
}

Time Time::now(){
    #if defined L_WINDOWS
        struct _timeb timebuffer;
        _ftime(&timebuffer);
        return Time(0,timebuffer.millitm,timebuffer.time);
    #elif defined L_UNIX
        struct timeval tv;
        gettimeofday(&tv,NULL);
        return Time(tv.tv_usec,0,tv.tv_sec);
    #endif
}
String Time::format(String str, Time t){
    time_t timestamp(t.seconds());
    char date[512];
    strftime(date,sizeof(date),str.c_str(),localtime(&timestamp));
    return date;
}
