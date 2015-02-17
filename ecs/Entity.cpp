#include "Entity.h"

using namespace L;
using namespace ECS;

VectorMap<Entity> Entity::entities;

uint Entity::add(const Entity& e){
    return entities.insert(e);
}
Entity& Entity::get(uint ei){
    return entities[ei];
}
