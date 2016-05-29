(local transform)
(local rigidbody)
(local start (fun (do
	(set transform (transform-require entity))
	(transform-rotate transform (vec 1 0 0) 1.57) ; PI/2 to face camera
	(set rigidbody (rigidbody-require entity))
	(sprite-load (sprite-add entity) "spritesheet.png")

	(local collider (collider-add entity))
	(collider-box collider)
)))
(local update (fun (do
	(local movement (delta * 4))
	(if (key-pressed UP) 		(transform-move transform (vec 0 movement 0)))
	(if (key-pressed LEFT) 	(transform-move transform (vec (0.0 - movement) 0 0)))
	(if (key-pressed DOWN)	(transform-move transform (vec 0 (0.0 - movement) 0)))
	(if (key-pressed RIGHT) (transform-move transform (vec movement 0 0)))
)))
