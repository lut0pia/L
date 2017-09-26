(set (self'start) (fun (do
	(self'entity | 'require-name || 'set | "camera")
	(set (self'transform) (self'entity | 'require-transform|))
	(self'transform | 'move | (vec 0 -20 10))
  (local camera (self'entity | 'require-camera|))
  (camera'perspective | 70 0.01 512)
	; Create cursor entity
	(set (self'cursor) (entity-make))
	(set (self'cursor-transform) (self'cursor | 'require-transform|))
	(self'cursor | 'require-primitive || 'material || 'parent | "material/sphere.lon")
	(self'cursor | 'require-primitive || 'scale | (vec 0.1))
	(self'entity | 'require-collider || 'sphere | 0.5)
	(self'entity | 'require-rigidbody || 'kinematic | true)
)))
(set (self'update) (fun (do
	; Camera movement
	(local transform (self'transform))
	(local cursor (self'cursor))
	(local cursor-transform (self'cursor-transform))
	(local movement (* real-delta 4))
	(if (button-pressed 'Shift) (set movement (* movement 4)))
	(if (or (button-pressed 'Z) (button-pressed 'W)) (transform'move | (vec 0 movement 0)))
	(if (or (button-pressed 'Q) (button-pressed 'A)) (transform'move | (vec (- movement) 0 0)))
	(if (button-pressed 'S) (transform'move | (vec 0 (- movement) 0)))
	(if (button-pressed 'D) (transform'move | (vec movement 0 0)))
	; Shooting
	(if (button-pressed 'LeftButton) (self'shoot|))
	; Cursor placing
	(local hit (raycast (transform'get-position|) (transform'forward|)))
	(if (non-null (hit'collider))
		(do
			(cursor'require-primitive || 'material || 'color | 'color (color 1.0 0 0 0.5))
			(cursor-transform'set-position | (hit'position))
		)
		(cursor'require-primitive || 'material || 'color | 'color (color 0 0 0 0))
	)
)))
(set (self'event) (fun (e) (do
	(local transform (self'transform))
	(switch (e'type)
		'MouseMove (do
			(transform'rotate | (vec 0 0 1) (* (e'x) -0.005))
			(transform'rotate | (vec 1 0 0) (* (e'y) -0.005))
		)
		'ButtonDown (switch (e'button)
			'Space (if (music-entity'require-midi-source || 'is-playing |)
				(music-entity'require-midi-source || 'stop |)
				(music-entity'require-midi-source || 'play |))
			'R (engine-clear-and-read "startup.ls")
		)
	)
)))
(set (self'shoot) (fun (do
	(if (> (- (now) (self'last-shoot)) (time 0.05)) (do
		(set (self'last-shoot) (now))
		(local e (entity-make))
		(local radius (vec 0.5 0.5 0.5))
		(e'require-transform || 'copy | (self'transform))
		(e'require-transform || 'move | (vec 0 0 1))
		(e'require-collider || 'box | radius)
		(e'require-primitive || 'material || 'parent | "material/box.lon")
		(e'require-primitive || 'material || 'color | 'color (rand-color))
		(e'require-primitive || 'scale | radius)
		(e'require-rigidbody || 'add-speed | (* (vec 16 16 16) (self'transform | 'forward|)))
	))
)))
(set (self'gui) (fun (do
	(draw-image 10 10 "logo.png")
	(draw-text 84 10
		(+ "FPS: " (/ 1.0 delta) "\n"
			 "Frame: " avg-frame-work-duration "\n"
		))
)))
