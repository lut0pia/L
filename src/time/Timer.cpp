#include "Timer.h"

using namespace L;

Timer::Timer() : _last(Time::now()), _paused(false) {}
void Timer::setoff() {
  _last = Time::now();
}
Time Timer::frame() {
  if(!_paused) {
    Time now(Time::now());
    Time wtr(now-_last);
    _last = now;
    return wtr;
  } else return 0;
}
Time Timer::since() const {
  return Time::now()-_last;
}
bool Timer::every(const Time& span) {
  Time now(Time::now());
  if(_last<now-span && !_paused) {
    _last = now;
    return true;
  } else return false;
}
void Timer::pause() {
  if(!_paused) {
    _last -= Time::now();
    _paused = true;
  }
}
void Timer::unpause() {
  if(_paused) {
    _last += Time::now();
    _paused = false;
  }
}
void Timer::togglePause() {
  if(_paused) unpause();
  else pause();
}

