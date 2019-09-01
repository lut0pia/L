(set rand_range (fun (min max) (+ min (* (- max min) (rand)))))
(set rand_color (fun (color (rand) (rand) (rand))))
(set make_box (fun (do
  (local entity (entity_make))
  (local transform (entity.require_transform))
  (local primitive (entity.add_primitive))
  (local light (entity.add_primitive))
  (transform.move
    (vec
      (rand_range -8 8)
      (rand_range -8 8)
      (rand_range 2 8)))
  (transform.rotate (vec 0 1 0) (* (- (rand) 0.5) 5))
  ; Add rigid body
  (entity.require_rigidbody|.add_velocity (vec (rand_range -2 2) (rand_range -2 2) (rand_range 1 2)))
  ; Add collider
  (if (< (rand) 0.0)
    (do
      (primitive.material|.parent "material/sphere.ls")
      (entity.require_collider|.sphere 0.5)
    )
    (do
      (primitive.material|.parent "material/box.ls")
      (entity.require_collider|.box (vec 0.5 0.5 0.5))
    )
  )
  (local color (rand_color))
  ; Geometry
  (primitive.scale (vec 0.5 0.5 0.5))
  (primitive.material|.color 'color color)
  ; Light
  (light.material|.parent "material/pointlight.ls")
  (light.material|.color 'color color)
  (light.material|.scalar 'intensity 4)
  (light.scale 32)
)))

(set make_cage (fun (do
  (local size 20)
  (local nsize (- size))
  (local half (/ size 2))
  (local nhalf (- half))
  (make_static_box (vec 0 0 0) (vec half half 0.5))
  (make_static_box (vec half 0 half) (vec 0.5 half half))
  (make_static_box (vec nhalf 0 half) (vec 0.5 half half))
  (make_static_box (vec 0 half half) (vec half 0.5 half))
  (make_static_box (vec 0 nhalf 1) (vec half 0.5 1))
)))
(set make_static_box (fun (position size) (do
  (local entity (entity_make))
  (entity.require_transform|.move position)
  (entity.require_collider|.box size)
  (entity.require_primitive|.scale size)
  (entity.require_primitive|.material|.parent "material/box.ls")
)))
(set make_mesh (fun (mat pos) (do
  (local entity (entity_make))
  (set truc entity)
  (local transform (entity.require_transform))
  (local primitive (entity.require_primitive))
  (transform.move pos)
  (primitive.material|.parent mat)
  entity
)))

(set scene_default (fun (do
  (engine_gravity (vec 0 0 -9.8))
  ;(engine_gravity (vec 0 0 0))
  ;(engine_timescale 0.1)
  (entity_make|.add_script|.load "camera.ls")
  (entity_make|.add_script|.load "sink.ls")

  ; Make ambient light
  (local amblight_entity (entity_make))
  (amblight_entity.require_primitive|.material|.parent "material/ssao.ls")
  (amblight_entity.require_primitive|.scale 99999)
  (amblight_entity.require_primitive|.material|.color 'color (color 0.2 0.2 0.2))

  ; Make directional light
  (local dirlight_entity (entity_make))
  (dirlight_entity.require_transform|.rotate (vec -1 0 1) 1)
  (dirlight_entity.require_primitive|.material|.parent "material/dirlight.ls")
  (dirlight_entity.require_primitive|.scale 99999)
  (dirlight_entity.require_primitive|.material|.scalar 'intensity 2)

  ; Make sky
  (local sky_entity (entity_make))
  (sky_entity.require_primitive|.material|.parent "material/sky.ls")
  (sky_entity.require_primitive|.scale 100)

  ; Make terrain
  (local terrain_size 32)
  (local terrain_entity (entity_make))
  (terrain_entity.require_transform|.move (vec -20 0 0))
  (terrain_entity.require_primitive|.scale (vec 10 10 2))
  (terrain_entity.require_primitive|.material|.parent "material/terrain.ls")

  (make_cage)

  (make_mesh "material/smartphone.ls" (vec -16 -20 5))
  (make_mesh "material/jerrican.ls" (vec 10 -16 5))
  (make_mesh "material/bush.ls" (vec -16 -28 0))
  (set truc (entity_copy truc))
  (truc.require_transform|.move (vec 30 0 0))

  (local sprite (entity_make))
  (sprite.require_transform|.move (vec -9.4 0 5))
  (sprite.require_transform|.rotate (vec 0 0 1) 1.57)
  (sprite.require_primitive|.material|.parent "material/sprite.ls")
  (sprite.require_primitive|.material|.texture 'tex "texture/bush.png?comp=bc3")
  (sprite.require_primitive|.scale (vec 5))

  ; Create all boxes
  (local box_count 16)
  (while (< 0 box_count) (do
    (make_box)
    (set box_count (- box_count 1))
  ))
)))

(scene_default)

(set music_entity (entity_make))
(music_entity.require_midi_source|.sequence "audio/mozart.mid")

(set sound_entity (entity_make))
(sound_entity.require_audio_source|.stream "audio/guitar.wav")
