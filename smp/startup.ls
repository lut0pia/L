(set rand-range (fun (min max) (+ min (* (- max min) (rand)))))
(set rand-color (fun (color (rand) (rand) (rand))))
(set make-box (fun (do
	(local entity (entity-make))
	(local transform ((entity'require-transform)))
	(transform'move |
		(vec
			(rand-range -8 8)
			(rand-range -8 8)
			(rand-range 2 8)))
	(transform'rotate | (vec 0 1 0) (* (- (rand) .5) 5))
	; Add rigid body
	(entity'require-rigidbody || 'add-speed | (vec (rand-range -2 2) (rand-range -2 2) (rand-range 1 2)))
	; Add collider
	(if (< (rand) 0.0)
		(do
			(entity'require-primitive || 'material || 'parent | "material/sphere.lon")
			(entity'require-collider || 'sphere | 0.5)
		)
		(do
			(entity'require-primitive || 'material || 'parent | "material/box.lon")
			(entity'require-collider || 'box | (vec 0.5 0.5 0.5))
		)
	)
	(entity'require-primitive || 'scale | (vec 0.5 0.5 0.5))
	(local color (rand-color))
	(entity'require-primitive || 'material ||  'color | 'color color)
	(entity'require-light || 'point | color 5 8)
)))

(set make-terrain (fun (do
	(local size 20)
	(local nsize (- size))
	(local half (/ size 2))
	(local nhalf (- half))
	(make-static-box (vec 0 0 0) (vec half half 0.5))
	(make-static-box (vec half 0 half) (vec 0.5 half half))
	(make-static-box (vec nhalf 0 half) (vec 0.5 half half))
	(make-static-box (vec 0 half half) (vec half 0.5 half))
	(make-static-box (vec 0 nhalf 1) (vec half 0.5 1))
)))
(set make-static-box (fun (position size) (do
	(local entity (entity-make))
	(entity'require-transform || 'move | position)
	(entity'require-collider || 'box | size)
	(entity'require-primitive || 'scale | size)
	(entity'require-primitive || 'material || 'parent | "material/box.lon")
)))
(set make-mesh (fun (mat pos) (do
	(local entity (entity-make))
	(set truc entity)
	(local transform (entity'require-transform|))
	(local primitive (entity'require-primitive|))
	(transform'move | pos)
	(primitive'material || 'parent | mat)
	entity
)))

(set scene-default (fun (do
	(engine-gravity (vec 0 0 -9.8))
	(light-pipeline ".inline?fragment=shader/light.frag&vertex=shader/light.vert&pass=light")
	(font-pipeline ".inline?fragment=shader/texture.frag&vertex=shader/font.vert&pass=present")
	;(engine-gravity (vec 0 0 0))
	;(engine-timescale 0.1)
	(entity-make | 'add-script || 'load | "camera.ls")
	(entity-make | 'add-script || 'load | "sink.ls")
	(entity-make | 'add-light || 'directional | (color) (vec -1 2 -3) 2)
	(make-terrain)

	(make-mesh "material/smartphone.lon" (vec -16 -20 5))
	(make-mesh "material/jerrican.lon" (vec 10 -16 5))
	(make-mesh "material/bush.lon" (vec -16 -28 0))
	(set truc (entity-copy truc))
	(truc'require-transform || 'move | (vec 30 0 0))

	;(entity-get "camera" | 'require-transform || 'move | (vec 0 0 10))
	;(entity-destroy (entity-get "camera"))

	(local sprite (entity-make))
	(sprite'require-transform || 'move | (vec -9.4 0 5))
	(sprite'require-transform || 'rotate | (vec 0 0 1) 1.57)
	(sprite'require-primitive || 'material || 'parent | "material/sprite.lon")
	(sprite'require-primitive || 'material || 'texture | 'tex "texture/bush.png")
	(sprite'require-primitive || 'scale | (vec 5))

	; Create all boxes
	(local box-count 16)
	(while (> box-count 0) (do
		(make-box)
		(set box-count (- box-count 1))
		))
)))

(scene-default)
;(local camera (entity-make))
;(camera'require-camera || 'ortho | -1 1 -1 1)

(set music-entity (entity-make))
(music-entity'require-midi-source || 'sequence | "audio/mozart.mid")

(set sound-entity (entity-make))
(sound-entity'require-audio-source || 'stream | "audio/guitar.wav")
