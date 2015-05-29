#ifndef DEF_L_Coroutine
#define DEF_L_Coroutine

#include <setjmp.h>
#include "../time/Time.h"
#include "../containers/StaticStack.h"
#include "../macros.h"

namespace L {
  class Coroutine {
    private:
      static StaticStack<32,Coroutine*> _coroutines;
      jmp_buf _caller, _callee;
      int _stack[4096];
      Time _nextJump, _nextYield;
      bool _returned;

    public:
      template <class F, class... Args>
      Coroutine(const F& f, Args&&... args) : _nextJump(0), _nextYield(0), _returned(false) {
        _coroutines.push(this);
        if(!setjmp(_caller)) {
#if defined L_X86_32
          void* newesp(_stack+4095);
          __asm {
            mov esp, newesp
          }
#else
#error
#endif
          proxy(f,args...);
        }
      }
      template <class F, class... Args>
      void proxy(F f, Args... args) {
        Coroutine::yield();
        f(args...);
        _returned = true;
        Coroutine::yield();
      }

      inline bool returned() const {return _returned;}

      void jump();
      void jumpFor(const Time&);
      static void yield();
      static void yieldFor(const Time&);
  };
}

#endif


