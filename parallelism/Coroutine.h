#ifndef DEF_L_Coroutine
#define DEF_L_Coroutine

#include "Fiber.h"
#include "../containers/StaticStack.h"
#include "../time/Time.h"

namespace L {
  class Coroutine {
    private:
      static StaticStack<32,Coroutine*> _coroutines;
      static Fiber* _mainFiber;
      Fiber* _fiber;
      Time _nextJump, _nextYield;
      bool _returned, _terminating;

    public:
      Coroutine(Fiber::Func f) : _nextJump(0), _nextYield(0), _returned(false), _terminating(false) {
        _coroutines.push(this);
        _fiber = Fiber::create(f);
      }

      inline bool returned() const {return _returned;}
      static bool terminating() {return _coroutines.top()->_terminating;}

      bool jump();
      bool jumpFor(const Time&);
      void terminate();
      static bool yield();
      static bool yieldFor(const Time&);

      inline static void convert() {_mainFiber = Fiber::convert();}
  };
}

#endif


