#include "Engine.h"

using namespace L;
using namespace ECS;

Set<void (*)()> Engine::_updates;

void Engine::addUpdate(void (*update)()) {
  _updates.insert(update);
}
void Engine::update() {
  for(int i(0); i<_updates.size(); i++)
    _updates[i]();
}
