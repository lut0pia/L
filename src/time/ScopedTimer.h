#pragma once

#include "Time.h"

namespace L {
  class ScopedTimer {
  protected:
    Time _start;
    Time& _counter;
  public:
    inline ScopedTimer(Time& counter) : _start(Time::now()), _counter(counter) {}
    inline ~ScopedTimer() { _counter += Time::now()-_start; }
  };
}