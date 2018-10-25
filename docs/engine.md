# Engine reference

This file documents how to interact with the engine in [script](script.md) in general.
For now, when the engine starts, it reads a script in the current directory named "startup.ls".

## Entities
```clojure
(local e1 (entity-make)) ; Create a new entity
(local e2 (entity-copy e1)) ; Copy an entity
(entity-destroy e1) ; Destroy an entity
```

## Physics
```clojure
(engine-gravity (vec 0 0 -9.8)) ; Sets the gravity (default is zero)
```
