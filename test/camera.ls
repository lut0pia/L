(local transform)
(local center-x (1920 / 2))
(local center-y (1080 / 2))

(local start (fun (do
	(set transform (transform-require entity))
	(transform-move transform (vec 0 -20 10))
  (local camera (camera-require entity))
  (camera-perspective camera 70 (16.0 / 9.0) 0.01 512)
	(mouse-set !center-x !center-y)
)))
(local update (fun (do
	; Camera rotation
	(local delta-x ((mouse-x) - !center-x))
	(local delta-y ((mouse-y) - !center-y))
	(transform-rotate transform (vec 0 0 1) (delta-x * -0.01))
	(transform-rotate transform (vec 1 0 0) (delta-y * -0.01))
	(mouse-set !center-x !center-y) ; Reset mouse cursor position
	; Camera movement
	(local movement (delta * 4))
	(if (key-pressed Z) (transform-move transform (vec 0 movement 0)))
	(if (key-pressed Q) (transform-move transform (vec (0 - movement) 0 0)))
	(if (key-pressed S) (transform-move transform (vec 0 (0 - movement) 0)))
	(if (key-pressed D) (transform-move transform (vec movement 0 0)))
)))
