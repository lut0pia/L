#include "Coroutine.h"

using namespace L;

StaticStack<32,Coroutine*> Coroutine::_coroutines;

void Coroutine::jump() {
  if(!_returned && (_nextJump==0 || _nextJump<Time::now())) {
    _coroutines.push(this);
    if(!setjmp(_caller))
      longjmp(_callee,1);
    _coroutines.pop();
  }
}
void Coroutine::jumpFor(const Time& time) {
  _nextYield = Time::now()+time;
  jump();
}
void Coroutine::yield() {
  if((_coroutines.top()->_nextYield==0 || _coroutines.top()->_nextYield<Time::now())
      && !setjmp(_coroutines.top()->_callee))
    longjmp(_coroutines.top()->_caller,1);
}
void Coroutine::yieldFor(const Time& time) {
  _coroutines.top()->_nextJump = Time::now()+time;
  yield();
}
