#include "Coroutine.h"

using namespace L;

StaticStack<32,Coroutine*> Coroutine::_coroutines;

bool Coroutine::jump() {
  if(!_returned && (_nextJump==0 || _nextJump<Time::now())) {
    _coroutines.push(this);
    if(!setjmp(_caller))
      longjmp(_callee,1);
    _coroutines.pop();
  }
  return _returned;
}
bool Coroutine::jumpFor(const Time& time) {
  _nextYield = Time::now()+time;
  return jump();
}
void Coroutine::terminate() {
  _terminating = true;
  while(!_returned) jump();
}
bool Coroutine::yield() {
  if((_coroutines.top()->_nextYield==0 || _coroutines.top()->_nextYield<Time::now())
      && !setjmp(_coroutines.top()->_callee))
    longjmp(_coroutines.top()->_caller,1);
  return _coroutines.top()->_terminating;
}
bool Coroutine::yieldFor(const Time& time) {
  _coroutines.top()->_nextJump = Time::now()+time;
  return yield();
}
