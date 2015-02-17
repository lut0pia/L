#include "Engine.h"

using namespace L;
using namespace ECS;

Set<void (*)()> Engine::updates;

void Engine::addUpdate(void (*update)()){
    updates.insert(update);
}
void Engine::update(){
    L_Iter(updates,update)
        (*update)();
}
