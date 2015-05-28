#include "Coroutine.h"

using namespace L;

StaticStack<32,Coroutine*> Coroutine::_coroutines;

void Coroutine::jump() {
  if(!_returned) {
    _coroutines.push(this);
    if(!setjmp(_caller))
      longjmp(_callee,1);
    _coroutines.pop();
  }
}
void Coroutine::jumpFor(const Time& time) {
  _time = Time::now()+time;
  jump();
}
void Coroutine::canJump() {
  if(_time<Time::now())
    jump();
}
void Coroutine::yield() {
  if(!setjmp(_coroutines.top()->_callee))
    longjmp(_coroutines.top()->_caller,1);
}
void Coroutine::yieldFor(const Time& time) {
  _coroutines.top()->_time = Time::now()+time;
  yield();
}
void Coroutine::canYield() {
  if(_coroutines.top()->_time<Time::now())
    yield();
}
