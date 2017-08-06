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
	(transform'rotate | !(vec 0 1 0) (* (- (rand) .5) 5))
	; Add rigid body
	(entity'require-rigidbody || 'add-speed | (vec (rand-range -2 2) (rand-range -2 2) (rand-range 1 2)))
	; Add collider
	(if (< (rand) 0)
		(do
			(entity'require-collider || 'sphere | 0.5)
			(entity'require-primitive || 'sphere | 0.5)
		)
		(do
			(entity'require-collider || 'box | !(vec 0.5 0.5 0.5))
			(entity'require-primitive || 'box | !(vec 0.5 0.5 0.5))
		)
	)
	(local color (rand-color))
	(entity'require-primitive || 'color | color)
	;(entity'require-light || 'spot | color (vec 0 1 0) 1 16 0.9)
	(entity'require-light || 'point | color 1 16)
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
	(entity'require-primitive || 'box | size)
)))
(set make-mesh (fun (mesh tex pos) (do
	(local entity (entity-make))
	(set truc entity)
	(local transform (entity'require-transform|))
	(local staticmesh (entity'require-staticmesh|))
	(transform'move | pos)
	(staticmesh'mesh | mesh)
	(staticmesh'texture | tex)
)))

(set scene-default (fun (do
	(engine-gravity (vec 0 0 -9.8))
	;(engine-gravity (vec 0 0 0))
	;(engine-timescale 0.1)
	(entity-make | 'add-script || 'load | "camera.ls")
	(entity-make | 'add-script || 'load | "sink.ls")
	(entity-make | 'add-light || 'directional | (color) (vec -1 2 -3) 1)
	(make-terrain)

	(make-mesh "smartphone.obj" "smartphone.png" (vec -16 -20 5))
	(make-mesh "jerrican.obj" "jerrican.png" (vec 4 -16 5))
	(make-mesh "bush.obj" "bush.png" (vec -16 -28 0))
	(set truc (entity-copy truc))
	(truc'require-transform || 'move | (vec 30 0 0))

	;(entity-get "camera" | 'require-transform || 'move | (vec 0 0 10))
	;(entity-destroy (entity-get "camera"))

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

;(local sprite (entity-make))
(sprite'require-transform || 'move | (vec 0 0 10))
(sprite'require-sprite || 'texture | "smartphone.png")
(sprite'require-sprite || 'vertex | -5 -5 5 5)
