# Components reference

This file documents the engine's components and how to interact with them in [script](script.md). Examples are in [`ls`](module/ls.md).

Each component can be accessed through three different functions from entities.
```clojure
(entity.add_component) ; Always adds a component to the entity and returns it
(entity.get_component) ; Returns the entity's component if it exists, null otherwise
(entity.require_component) ; Either adds or returns the component if it already exists
```
Most of the time you will want to use `require_component` because it is safer and you
rarely want multiple instances of the same component on an entity.
```clojure
(component.entity) ; Returns the entity of a component
```

## Transform
Transform components are used to express location and rotation of objects.
They're often required by components like meshes or colliders.
The basis convention is X as right, Y as forward and Z as up.
```clojure
(transform.move (vec 4 2 6)) ; Moves transform relatively (not absolute)
(transform.rotate (euler_degrees 90 0 0)) ; Rotates transform using euler angles (probably counter-clockwise?)
```

## Script
Script components are used to add behaviour to entities. Their `start` function is called when they're added to an entity, their `update` function is called every frame, their `update_components` function is called each time components have been added to or removed of their entity, and their `event` function is called for different events including input events. They have a local variable `entity`.
```clojure
(script.load "some/script.ls") ; Loads script.ls in the component
(script.object) ; Gets the self object inside the component
```
Example of *script.ls*:
```clojure
(set self.start (fun (do
  (set self.transform (self.entity.require_transform))
)))
(set self.event (fun e (do
  (switch e.type
    'Collision (if (= e.other bullet_collider) (die))
  )
)))
```

## Camera
Camera components link regions of the screen to projections of the rendered world.
```clojure
(camera.present_material "material/present.ls") ; Sets the material used to present from GBuffer to final screen buffer
(camera.perspective fovy ratio far near) ; Generates perspective projection matrix
(camera.ortho left right bottom top) ; Generates orthographic projection matrix (near is -1 and far is 1)
(camera.screen_to_ray (vec -0.5 0)) ; Gets absolute ray from normalized screen position
(camera.screen_to_pixel (vec -0.5 0)) ; Gets pixel position from normalized screen position
(camera.pixel_to_screen (vec 512 128)) ; Gets normalized screen position from pixel position
```

## Collider
Collider components are used to collide with other colliders (and make rigidbodies react).
```clojure
(collider.center (vec 2 1 4)) ; Makes the shape have a relative offset
(collider.box (vec 2 1 4)) ; Makes a box collider with the vector as radius
(collider.sphere 1.5) ; Makes a sphere collider with the float as radius
```

## Primitive
Primitive components hold a material rendered at a scale.
```clojure
(primitive.material) ; Gets the material of the primitive
(primitive.scale (vec 2 2 1)) ; Sets the scale of the primitive
```

## RigidBody
RigidBody components allow for an entity to be physics-driven. Which means it will
be affected by gravity and collide with other rigid bodies and static colliders.
```clojure
(rigidbody.kinematic true) ; Sets the body as kinematic (unaffected by external forces)
(rigidbody.mass 1.5) ; Sets the mass of the body
(rigidbody.restitution 0.8) ; Sets the restitution (bounciness) of the body
(rigidbody.drag 0.8) ; Sets the drag of the body (0 is none)
(rigidbody.angular_drag 0.8) ; Sets the angular drag of the body (0 is none)
(rigidbody.get_speed) ; Gets the absolute velocity of the body
(rigidbody.get_relative_speed) ; Gets the relative velocity of the body
(rigidbody.get_velocity_at (vec 0 1 0)) ; Gets the absolute velocity of the body at specific point
(rigidbody.add_velocity (vec 0 4 0)) ; Simply adds velocity to the body
(rigidbody.add_force (vec 0 4 0)) ; Adds an absolute force to the body
(rigidbody.add_relative_force (vec 0 4 0)) ; Adds a relative absolute force to the body
(rigidbody.add_torque (vec 0 4 0)) ; Adds an absolute torque to the body
(rigidbody.add_relative_torque (vec 0 4 0)) ; Adds a relative torque to the body
```
