(local transform)

(local start (fun (do
	(set transform (transform-require entity))
	(transform-move transform (vec 0 -20 10))
  (local camera (camera-require entity))
  (camera-perspective camera 70 (16.0 / 9.0) 0.01 512)
)))
(local update (fun (do
	; Camera movement
	(local movement (delta * 4))
	(if (key-pressed Z) (transform-move transform (vec 0 movement 0)))
	(if (key-pressed Q) (transform-move transform (vec (0 - movement) 0 0)))
	(if (key-pressed S) (transform-move transform (vec 0 (0 - movement) 0)))
	(if (key-pressed D) (transform-move transform (vec movement 0 0)))
)))
(local event (fun (e) (do
	(if ((e'type) = 'MOUSEMOVE) (do
		(transform-rotate transform (vec 0 0 1) ((e'x) * -0.01))
		(transform-rotate transform (vec 1 0 0) ((e'y) * -0.01))
	))
)))
