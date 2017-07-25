# Components reference

This file documents the engine's components and how to interact with them in [script](script.md).

Each component can be accessed through three different functions from entities.
```clojure
(entity'add-component |) ; Always adds a component to the entity and returns it
(entity'get-component |) ; Returns the entity's component if it exists, null otherwise
(entity'require-component |) ; Either adds or returns the component if it already exists
```
Most of the time you will want to use `require-component` because it is safer and you
rarely want multiple instances of the same component on an entity.
```clojure
(component'entity |) ; Returns the entity of a component
```

## Transform
Transform components are used to express location and rotation of objects.
They're often required by components like meshes or colliders.
The basis convention is X as right, Y as forward and Z as up.
```clojure
(transform'move | (vec 4 2 6)) ; Moves transform relatively (not absolute)
(transform'rotate | (vec 1 0 0) angle) ; Rotates transform around the axis by angle (probably counter-clockwise?)
```

## Script
Script components are used to add behaviour to entities. Their `start` function is called when they're added to an entity, their `update` function is called every frame, their `update-components` function is called each time components have been added to or removed of their entity, and their `event` function is called for different events including input events. They have a local variable `entity`.
```clojure
(script'load | "some/script.ls") ; Loads script.ls in the component
```
Example of *script.ls*:
```clojure
(set (self'start) (fun (do
  (set (self'transform) (entity'require-transform|))
)))
(set (self'event) (fun (e) (do
  (switch (e'type)
    'BUTTONDOWN (if (= (e'button) 'SPACE) (jump))
    'BUTTONUP (if (= (e'button) 'SPACE) (stop-jump))
    'COLLISION (if (= (e'other) bullet-collider) (die))
    'MOUSEMOVE (move (e'x) (e'y))
  )
)))
```

## Camera
Camera components link regions of the screen to projections of the rendered world.
```clojure
(camera'perspective | fovy ratio far near) ; Generates perspective projection matrix
(camera'ortho | left right bottom top) ; Generates orthographic projection matrix (near is -1 and far is 1)
```

## StaticMesh
StaticMesh components are used to render basic meshes in a scene.
```clojure
(staticmesh'mesh | "some/path.obj") ; Loads path.obj as a mesh
(staticmesh'texture | "some/path.png") ; Loads path.png as a texture
```

## Collider
Collider components are used to collide with other colliders (and make rigidbodies react).
```clojure
(collider'center | (vec 2 1 4)) ; Makes the shape have a relative offset
(collider'box | (vec 2 1 4)) ; Makes a box collider with the vector as radius
(collider'sphere | 1.5) ; Makes a sphere collider with the float as radius
```

## Primitive
Primitive components are simple rendering shapes. They're white, ugly, and made for debugging.
```clojure
(primitive'center | (vec 2 1 4)) ; Makes the shape have a relative offset
(primitive'box | (vec 2 1 4)) ; Makes a solid box shape with the vector as radius
(primitive'sphere | 1.5) ; Makes a solid sphere shape with the float as radius
```

## RigidBody
RigidBody components allow for an entity to be physics-driven. Which means it will
be affected by gravity and collide with other rigid bodies and static colliders.
```clojure
(rigidbody'mass | 1.5) ; Sets the mass of the body
(rigidbody'restitution | 0.8) ; Sets the restitution (bounciness) of the body
(rigidbody'drag | 0.8) ; Sets the drag of the body (0 is none)
(rigidbody'angular-drag | 0.8) ; Sets the angular drag of the body (0 is none)
(rigidbody'add-speed | (vec 0 4 0)) ; Simply adds velocity to the body
(rigidbody'add-force | (vec 0 4 0)) ; Adds an absolute force to the body
(rigidbody'add-relative-force | (vec 0 4 0)) ; Adds a relative absolute force to the body
(rigidbody'add-torque | (vec 0 4 0)) ; Adds an absolute torque to the body
(rigidbody'add-relative-torque | (vec 0 4 0)) ; Adds a relative torque to the body
```
