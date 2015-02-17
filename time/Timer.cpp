#include "Timer.h"

using namespace L;

Timer::Timer() : last(Time::now()), paused(false){}
void Timer::setoff(){
    last = Time::now();
}
Time Timer::frame(){
    if(!paused){
        Time now(Time::now());
        Time wtr(now-last);
        last = now;
        return wtr;
    }
    else return 0;
}
Time Timer::since() const{
    return Time::now()-last;
}
bool Timer::every(const Time& span){
    Time now(Time::now());
    if(last<now-span && !paused){
        last = now;
        return true;
    }
    else return false;
}
void Timer::pause(){
    if(!paused){
        last -= Time::now();
        paused = true;
    }
}
void Timer::unpause(){
    if(paused){
        last += Time::now();
        paused = false;
    }
}
void Timer::togglePause(){
    if(paused)  unpause();
    else        pause();
}
bool Timer::gPaused(){
    return paused;
}

