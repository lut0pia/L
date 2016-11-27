#include "Fiber.h"

#include <windows.h>
#include "../containers/Pool.h"
#include "../macros.h"

using namespace L;

#ifdef L_WINDOWS
VOID __stdcall proxy(LPVOID p) { // Proxy is needed because of calling conventions
  Fiber* fiber((Fiber*)p); // Potential trouble if Fiber has moved before being switched to
  fiber->func()(fiber);
  fiber->remove();
}
#endif

Fiber::Fiber(Func f,void* p) : _func(f),_param(p),_over(false) {
#ifdef L_WINDOWS
  if(!(_handle = CreateFiber(0,proxy,this)))
    L_ERROR("Creation of fiber failed.");
#endif
}

void Fiber::schedule() {
#ifdef L_WINDOWS
  SwitchToFiber(_handle);
#endif
}
void Fiber::yield() {
  Fiber* next(nullptr);
  for(auto&& fiber : Pool<Fiber>::global)
    if(!fiber._over)
      next = &fiber;
  if(next) next->schedule();
  else L_ERROR("No more fibers in the pool");
}
void Fiber::remove() {
  Pool<Fiber>::global.destruct(this);
  _over = true;
  yield();
}

Fiber* Fiber::create(Func f,void* p) {
  return Pool<Fiber>::global.construct(f,p);
}
Fiber* Fiber::convert() {
  Fiber* fiber(Pool<Fiber>::global.allocate());
  fiber->_over = false;
#ifdef L_WINDOWS
  fiber->_handle = ConvertThreadToFiber(0);
#endif
  return fiber;
}
