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
	(local movement (delta * 8))
	(if (key-pressed UP) 		(rigidbody-addspeed rigidbody (vec 0 0 movement)))
	(if (key-pressed LEFT) 	(rigidbody-addspeed rigidbody (vec (0.0 - movement) 0 0)))
	(if (key-pressed DOWN)	(rigidbody-addspeed rigidbody (vec 0 0 (0.0 - movement))))
	(if (key-pressed RIGHT) (rigidbody-addspeed rigidbody (vec movement 0 0)))
)))
