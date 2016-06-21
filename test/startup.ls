(local make-box (fun (do
	(local entity (entity-make))
	(local rigidbody (rigidbody-add entity))
	(local collider (collider-add entity))
	(local transform (transform-get entity))
	(collider-box collider)
	(transform-move transform
		(vec
			(((rand) - .5) * 16)
			(((rand) - .5) * 16)
			((rand) * 4)))
	(transform-rotate transform (vec 0 1 0) (((rand) - .5) * 5))
)))

(local make-terrain (fun (do
	(local entity (entity-make))
	(local transform (transform-require entity))
	(local collider (collider-require entity))
	(collider-box collider (vec 16 16 1))
	(transform-move transform (vec 0 0 -4))
	(transform-rotate transform (vec 0 1 0) -0.5)
	(set entity (entity-copy entity))
	(set transform (transform-require entity))
	(transform-move transform (vec -4 0 0))
	(transform-rotate transform (vec 0 1 0) 1)
)))

(script-load (script-add (entity-make)) "camera.ls")
(script-load (script-add (entity-make)) "character.ls")
(make-terrain)

; Create all boxes
(local box-count 8)
(while (box-count > 0) (do
	(make-box)
	(set box-count (box-count - 1))
))
