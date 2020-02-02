# Engine reference

This file documents how to interact with the engine in [script](script.md) in general.
For now, when the engine starts, it reads a script in the current directory named "ini.ls".

## Entities
* `entity_make()` takes no argument and returns a new entity
* `entity_copy(e)` takes an entity as argument and returns a new entity copied from the supplied entity
* `entity_destroy(e)` takes an entity as argument and queues it for destruction

## Physics
* `engine_gravity(v)` takes a vector in meter per second per second and sets the global gravity to that
