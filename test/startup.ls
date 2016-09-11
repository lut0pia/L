(set rand-range (fun (min max) (+ min (* (- max min) (rand)))))
(set make-box (fun (do
	(local entity (entity-make))
	(local rigidbody ((entity'add-rigidbody)))
	(local collider ((entity'add-collider)))
	(local transform ((entity'require-transform)))
	(if (< (rand) 0.5)
		(collider'sphere | 0.5)
		(collider'box | (vec 0.5 0.5 0.5)))
	(transform'move |
		(vec
			(* (- (rand) 0.5) 16)
			(* (- (rand) 0.5) 16)
			(* (rand) 16.0)))
	(transform'rotate | (vec 0 1 0) (* (- (rand) .5) 5))
	(rigidbody'add-speed | (vec (rand-range -5 5) (rand-range -5 5) (rand-range 2.5 5)))
)))

(local make-terrain (fun (do
	(local size 20)
	(local nsize (- size))
	(local half (/ size 2))
	(local nhalf (- half))
	(make-static-box (vec 0 0 0) (vec half half 1))
	(make-static-box (vec 0 0 size) (vec half half 1))
	(make-static-box (vec half 0 half) (vec 1 half half))
	(make-static-box (vec nhalf 0 half) (vec 1 half half))
	(make-static-box (vec 0 half half) (vec half 1 half))
	;(make-static-box (vec 0 nhalf half) (vec size 1 size))
)))
(local make-static-box (fun (position size) (do
	(local entity (entity-make))
	(entity'require-transform || 'move | position)
	(entity'require-collider || 'box | size)
)))
(local make-mesh (fun (mesh tex pos) (do
	(local entity (entity-make))
	(local transform (entity'require-transform|))
	(local staticmesh (entity'require-staticmesh|))
	(transform'move | pos)
	(staticmesh'mesh | mesh)
	(staticmesh'texture | tex)
)))

(engine-gravity (vec 0 0 -9.8))
(entity-make | 'add-script || 'load | "camera.ls")
(make-terrain)

(make-mesh "smartphone.obj" "smartphone.png" (vec 0 0 10))
(make-mesh "jerrican.obj" "jerrican.png" (vec 0 0 0))
(make-mesh "bush.obj" "bush.png" (vec 0 0 0))

; Create all boxes
(local box-count 16)
(while (> box-count 0) (do
	(make-box)
	(set box-count (- box-count 1))
))
