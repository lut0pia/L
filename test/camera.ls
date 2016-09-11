(local transform)

(local start (fun (do
	(set transform ((entity'require-transform)))
	(transform'move | (vec 0 -20 10))
  (local camera ((entity'require-camera)))
  (camera'perspective | 70 (/ 16.0 9.0) 0.01 512)
)))
(local update (fun (do
	; Camera movement
	(local movement (* delta 4))
	(if (key-pressed Z) (transform'move | (vec 0 movement 0)))
	(if (key-pressed Q) (transform'move | (vec (- movement) 0 0)))
	(if (key-pressed S) (transform'move | (vec 0 (- movement) 0)))
	(if (key-pressed D) (transform'move | (vec movement 0 0)))
)))
(local event (fun (e) (do
	(if (= (e'type) 'MOUSEMOVE) (do
		(transform'rotate | (vec 0 0 1) (* (e'x) -0.005))
		(transform'rotate | (vec 1 0 0) (* (e'y) -0.005))
	))
)))
