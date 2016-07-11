(local make-box (fun (do
	(local entity (entity-make))
	(local rigidbody (rigidbody-add entity))
	(local collider (collider-add entity))
	(local transform (transform-require entity))
	(collider-box collider)
	(transform-move transform
		(vec
			(((rand) - .5) * 16)
			(((rand) - .5) * 16)
			((rand) * 16.0)))
	(transform-rotate transform (vec 0 1 0) (((rand) - .5) * 5))
	(rigidbody-addspeed rigidbody (vec 0 0 ((rand) * 10.)))
)))

(local make-terrain (fun (do
	(local size 20)
	(local nsize (0 - size))
	(local half (size / 2))
	(local nhalf (0 - half))
	(make-static-box (vec 0 0 0) (vec size size 1))
	(make-static-box (vec 0 0 size) (vec size size 1))
	(make-static-box (vec half 0 half) (vec 1 size size))
	(make-static-box (vec nhalf 0 half) (vec 1 size size))
	(make-static-box (vec 0 half half) (vec size 1 size))
	(make-static-box (vec 0 nhalf half) (vec size 1 size))
)))
(local make-static-box (fun (position size) (do
	(local entity (entity-make))
	(transform-move (transform-require entity) position)
	(collider-box (collider-require entity) size)
)))
(local make-mesh (fun (mesh tex pos) (do
	(local entity (entity-make))
	(local entity-transform (transform-require entity))
	(local entity-mesh (staticmesh-require entity))
	(transform-move entity-transform pos)
	(staticmesh-mesh entity-mesh mesh)
	(staticmesh-texture entity-mesh tex)
)))

(script-load (script-add (entity-make)) "camera.ls")
(script-load (script-add (entity-make)) "character.ls")
(make-terrain)

(make-mesh "smartphone.obj" "smartphone.png" (vec 0 0 10))
(make-mesh "jerrican.obj" "jerrican.png" (vec 0 0 0))

; Create all boxes
(local box-count 8)
(while (box-count > 0) (do
	(make-box)
	(set box-count (box-count - 1))
))
