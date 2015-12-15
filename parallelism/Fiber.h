#ifndef DEF_L_Fiber
#define DEF_L_Fiber

#include "../systems.h"
#include "../containers/Pool.h"

namespace L {
  class Fiber {
    public:
      typedef void (*Func)(Fiber*);
    private:
      static Pool<Fiber> _pool;

      Func _func;
      void* _param;
      bool _over;
#ifdef L_WINDOWS
      LPVOID _handle;
#endif
    public:
      Fiber(Func f, void* p = 0);

      void schedule();
      void yield();
      void remove();

      inline Func func() const {return _func;}
      inline void* param() const {return _param;}

      static Fiber* create(Func, void* = 0);
      static Fiber* convert();
  };
}

#endif


