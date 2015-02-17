#ifndef DEF_L_Timer
#define DEF_L_Timer

#include "Time.h"

namespace L{
    class Timer{
        private:
            Time last;
            bool paused;

        public:
            Timer();
            void setoff();
            Time frame();
            Time since() const;
            bool every(const Time& span);
            void pause();
            void unpause();
            void togglePause();
            bool gPaused();
    };
}

#endif






