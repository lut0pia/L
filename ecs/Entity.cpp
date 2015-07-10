#include "Entity.h"

using namespace L;
using namespace ECS;

Set<void (*)()> Entity::_updates;
