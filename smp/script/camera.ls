(set self.start (fun (do
  (self.entity.require_name|.set "camera")
  (set self.transform (self.entity.require_transform))
  (self.transform.move (vec 0 -20 10))
  (local camera (self.entity.require_camera))
  (camera.perspective 70 0.1 4096)
  ; Create present material
  (camera.present_material|.shader 'fragment "shader/present.frag")
  (camera.present_material|.shader 'vertex "shader/fullscreen.vert")
  (camera.present_material|.render_pass 'present)
  (camera.present_material|.vertex_count 3)
  ; Create cursor entity
  (set self.cursor (entity_make))
  (set self.cursor_transform (self.cursor.require_transform))
  (self.cursor.require_primitive|.material|.parent "material/sphere.ls")
  (self.cursor.require_primitive|.scale (vec 0.1))
  (self.entity.require_collider|.sphere 0.5)
  (self.entity.require_rigidbody|.kinematic true)
  ; Create debug text
  (set self.text_gui (self.entity.add_gui))
  (self.text_gui.material|.parent "material/pixel_text.ls")
  (self.text_gui.offset 10 -10)
  (self.text_gui.anchor 0 1)
  (self.text_gui.viewport_anchor 0 1)
  (self.text_gui.scale 18 18)
  ; Create logo
  (set self.logo_gui (self.entity.add_gui))
  (self.logo_gui.material|.shader 'fragment "shader/texture.frag")
  (self.logo_gui.material|.shader 'vertex "shader/quad.vert")
  (self.logo_gui.material|.render_pass 'present)
  (self.logo_gui.material|.texture 'tex "texture/logo.png?comp=bc3")
  (self.logo_gui.material|.vertex_count 6)
  (self.logo_gui.offset 10 10)
)))
(set self.update (fun (do
  ; Movement values
  (local transform self.transform)
  (local cursor self.cursor)
  (local cursor_transform self.cursor_transform)
  (local movement (* real_delta 16))
  (local rotation (* real_delta 2))
  (local axis_x 0)
  (local axis_y 0)
  (local axis_rot_x 0)
  (local axis_rot_y 0)
  (local should_shoot false)

  ; Gather joystick input
  (foreach device _ (get_devices) (do
    (+= axis_x (device.get_axis 'GamepadLeftStickX))
    (+= axis_y (device.get_axis 'GamepadLeftStickY))
    (+= axis_rot_x (* -1 (device.get_axis 'GamepadRightStickX)))
    (+= axis_rot_y (* 1 (device.get_axis 'GamepadRightStickY)))
    (+= axis_rot_x (* -0.1 (device.get_axis 'MouseDX)))
    (+= axis_rot_y (* -0.1 (device.get_axis 'MouseDY)))
    (if (device.get_button 'GamepadFaceBottom) (set should_shoot true))
  ))

  ; Gather keyboard input
  (if (button_pressed 'LeftShift) (/= movement 4))
  (if (or (button_pressed 'Z) (button_pressed 'W)) (+= axis_y 1))
  (if (or (button_pressed 'Q) (button_pressed 'A)) (-= axis_x 1))
  (if (button_pressed 'S) (-= axis_y 1))
  (if (button_pressed 'D) (+= axis_x 1))
  (if (button_pressed 'MouseLeft) (set should_shoot true))

  ; Execute input
  (transform.move (vec (* axis_x movement) 0 0))
  (transform.move (vec 0 (* axis_y movement) 0))
  (transform.rotate (euler_radians 0 0 (* axis_rot_x rotation)))
  (transform.rotate (euler_radians (* axis_rot_y rotation) 0 0))
  (if should_shoot (self.shoot))

  ; Cursor placing
  (local hit (raycast (transform.get_position) (transform.forward)))
  (if (non_null hit.collider)
    (do
      (cursor.require_primitive|.material|.color 'color (color 1.0 0 0 0.5))
      (cursor_transform.set_position hit.position)
    )
    (cursor.require_primitive|.material|.color 'color (color 0 0 0 0))
  )

  ; GUI
  (self.text_gui.material|.text
    (+ "FPS: " (/ 1.0 delta) "\n"
    "Frame: " avg_frame_work_duration "\n"))
)))
(set self.event (fun e (do
  (if e.pressed (switch e.button
    'B (if (bach_source.is_playing)
      (bach_source.stop)
      (bach_source.play))
    'G (if (guitar_source.is_playing)
      (guitar_source.stop)
      (guitar_source.play))
    'M (if (mozart_source.is_playing)
      (mozart_source.stop)
      (mozart_source.play))
    'R (engine_clear_and_read "script/startup.ls")
  ))
)))
(set self.shoot (fun (do
  (if (> (- (now) self.last_shoot) (time 0.05)) (do
    (set self.last_shoot (now))
    (local e (entity_make))
    (local radius (vec 0.5 0.5 0.5))
    (e.require_transform|.copy self.transform)
    (e.require_transform|.move (vec 0 0 1))
    (e.require_collider|.box radius)
    (e.require_primitive|.material|.parent "material/box.ls")
    (e.require_primitive|.material|.color 'color (rand_color))
    (e.require_primitive|.scale radius)
    (e.require_rigidbody|.add_velocity (* (vec 16 16 16) (self.transform.forward)))
  ))
)))
