#include "Fiber.h"

#include <cstdio>
#include "../macros.h"
#include "../streams/Stream.h"

using namespace L;

Pool<Fiber> Fiber::_pool;

#ifdef L_WINDOWS
VOID __stdcall proxy(LPVOID p) { // Proxy is needed because of calling conventions
  Fiber* fiber((Fiber*)p); // Potential trouble if Fiber has moved before being switched to
  fiber->func()(fiber);
  fiber->remove();
}
#endif

Fiber::Fiber(Func f, void* p) : _func(f), _param(p), _over(false) {
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
  Fiber* next(0);
  for(auto&& fiber : _pool)
    if(!fiber._over)
      next = &fiber;
  if(next) next->schedule();
  else L_ERROR("No more fibers in the pool");
}
void Fiber::remove() {
  _pool.destruct(this);
  _over = true;
  yield();
}

Fiber* Fiber::create(Func f, void* p) {
  return _pool.construct(f,p);
}
Fiber* Fiber::convert() {
  Fiber* fiber(_pool.allocate());
  fiber->_over = false;
#ifdef L_WINDOWS
  fiber->_handle = ConvertThreadToFiber(0);
#endif
  return fiber;
}
