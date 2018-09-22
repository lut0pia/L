(set (self'start) (fun (do
	(self'entity | 'require-name || 'set | "camera")
	(set (self'transform) (self'entity | 'require-transform|))
	(self'transform | 'move | (vec 0 -20 10))
  (local camera (self'entity | 'require-camera|))
  (camera'perspective | 70 0.01 512)
	; Create cursor entity
	(set (self'cursor) (entity-make))
	(set (self'cursor-transform) (self'cursor | 'require-transform|))
	(self'cursor | 'require-primitive || 'material || 'parent | "material/sphere.ls")
	(self'cursor | 'require-primitive || 'scale | (vec 0.1))
	(self'entity | 'require-collider || 'sphere | 0.5)
	(self'entity | 'require-rigidbody || 'kinematic | true)
	; Create debug text
	(set (self'text-gui) (self'entity'add-gui|))
	(self'text-gui'material || 'pipeline | ".inline?fragment=shader/font.frag&vertex=shader/font.vert&pass=present")
	(self'text-gui'material || 'font | ".pixel")
	(self'text-gui'material || 'color | 'color "white")
	(self'text-gui'offset | 10 -10)
	(self'text-gui'anchor | 0 1)
	(self'text-gui'viewport-anchor | 0 1)
	(self'text-gui'scale | 18 18)
	; Create logo
	(set (self'logo-gui) (self'entity'add-gui|))
	(self'logo-gui'material || 'pipeline | ".inline?fragment=shader/texture.frag&vertex=shader/quad.vert&pass=present")
	(self'logo-gui'material || 'texture | 'tex "texture/logo.png?comp=bc3")
	(self'logo-gui'material || 'vertex-count | 6)
	(self'logo-gui'offset | 10 10)
)))
(set (self'update) (fun (do
	; Movement values
	(local transform (self'transform))
	(local cursor (self'cursor))
	(local cursor-transform (self'cursor-transform))
	(local movement (* real-delta 16))
	(local rotation (* real-delta 2))
	(local axis-x 0)
	(local axis-y 0)
	(local axis-rot-x 0)
	(local axis-rot-y 0)
	(local should-shoot false)

	; Gather joystick input
	(foreach device _ (get-devices) (do
		(+= axis-x (device'get-axis | 'GamepadLeftStickX))
		(+= axis-y (device'get-axis | 'GamepadLeftStickY))
		(+= axis-rot-x (* -1 (device'get-axis | 'GamepadRightStickX)))
		(+= axis-rot-y (* 1 (device'get-axis | 'GamepadRightStickY)))
		(+= axis-rot-x (* -0.1 (device'get-axis | 'MouseDX)))
		(+= axis-rot-y (* -0.1 (device'get-axis | 'MouseDY)))
		(if (device'get-button | 'GamepadFaceBottom) (set should-shoot true))
		(if (> (device'get-axis | 5) 0) (set should-shoot true))
	))
	
	; Gather keyboard input
	(if (button-pressed 'LeftShift) (/= movement 4))
	(if (or (button-pressed 'Z) (button-pressed 'W)) (+= axis-y 1))
	(if (or (button-pressed 'Q) (button-pressed 'A)) (-= axis-x 1))
	(if (button-pressed 'S) (-= axis-y 1))
	(if (button-pressed 'D) (+= axis-x 1))
	(if (button-pressed 'MouseLeft) (set should-shoot true))

	; Execute input
	(transform'move | (vec (* axis-x movement) 0 0))
	(transform'move | (vec 0 (* axis-y movement) 0))
	(transform'rotate | (vec 0 0 1) (* axis-rot-x rotation))
	(transform'rotate | (vec 1 0 0) (* axis-rot-y rotation))
	(if should-shoot (self'shoot|))

	; Cursor placing
	(local hit (raycast (transform'get-position|) (transform'forward|)))
	(if (non-null (hit'collider))
		(do
			(cursor'require-primitive || 'material || 'color | 'color (color 1.0 0 0 0.5))
			(cursor-transform'set-position | (hit'position))
		)
		(cursor'require-primitive || 'material || 'color | 'color (color 0 0 0 0))
	)

	; GUI
	(self'text-gui'material || 'text |
		(+ "FPS: " (/ 1.0 delta) "\n"
		"Frame: " avg-frame-work-duration "\n"))

)))
(set (self'event) (fun (e) (do
	(if (e'pressed) (switch (e'button)
		'Space (if (music-entity'require-midi-source || 'is-playing |)
			(music-entity'require-midi-source || 'stop |)
			(music-entity'require-midi-source || 'play |))
		'G (if (not (sound-entity'require-audio-source || 'is-playing |))
			(sound-entity'require-audio-source || 'play |))
		'R (engine-clear-and-read "startup.ls")
	))
)))
(set (self'shoot) (fun (do
	(if (> (- (now) (self'last-shoot)) (time 0.05)) (do
		(set (self'last-shoot) (now))
		(local e (entity-make))
		(local radius (vec 0.5 0.5 0.5))
		(e'require-transform || 'copy | (self'transform))
		(e'require-transform || 'move | (vec 0 0 1))
		(e'require-collider || 'box | radius)
		(e'require-primitive || 'material || 'parent | "material/box.ls")
		(e'require-primitive || 'material || 'color | 'color (rand-color))
		(e'require-primitive || 'scale | radius)
		(e'require-rigidbody || 'add-speed | (* (vec 16 16 16) (self'transform | 'forward|)))
	))
)))
