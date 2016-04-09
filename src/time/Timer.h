#ifndef DEF_L_Timer
#define DEF_L_Timer

#include "Time.h"

namespace L {
  class Timer {
    private:
      Time _last;
      bool _paused;

    public:
      Timer();

      inline bool paused() const {return _paused;}

      void setoff();
      Time frame();
      Time since() const;
      bool every(const Time& span);
      void pause();
      void unpause();
      void togglePause();
  };
}

#endif






