(set self.start (fn (do
  (self.entity.require_name|.set "camera")
  (set self.last_shoot (time 0))
  (set self.transform (self.entity.require_transform))
  (self.transform.move (vec 0 -20 10))
  (local camera (self.entity.require_camera))
  (camera.perspective 70 0.1 4096)
  ; Create input component, input context and input map
  (set self.input (self.entity.require_input|.context))
  (self.input.set_input_map ((read "script/input_map.ls")))
  (self.input.set_name "camera")
  ; Create present material
  (camera.present_material|.shader 'fragment "shader/present.frag")
  (camera.present_material|.shader 'vertex "shader/fullscreen.vert")
  (camera.present_material|.render_pass 'present)
  (camera.present_material|.vertex_count 3)
  ; Create cursor entity
  (set self.cursor (entity_make))
  (set self.cursor_transform (self.cursor.require_transform))
  (self.cursor.require_primitive|.material|.parent "material/sphere_light.ls")
  (self.cursor.require_primitive|.scale 0.1)
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
  (self.logo_gui.material|.texture 'tex "texture/logo.ls")
  (self.logo_gui.material|.vertex_count 6)
  (self.logo_gui.offset 10 10)
)))
(set self.update (fn (do
  ; Movement values
  (local transform self.transform)
  (local cursor self.cursor)
  (local cursor_transform self.cursor_transform)
  (local input self.input)
  (local movement (* real_delta 16))
  (if (input.get_button 'Slow) (/= movement 4))
  (local rotation (* real_delta 2))
  (local axis_x (input.get_axis 'XAxis))
  (local axis_y (input.get_axis 'YAxis))
  (local axis_rot_x (input.get_axis 'XAxisRot))
  (local axis_rot_y (input.get_axis 'YAxisRot))

  ; Execute input
  (transform.move (vec (* axis_x movement) 0 0))
  (transform.move (vec 0 (* axis_y movement) 0))
  (transform.rotate (euler_radians 0 0 (* axis_rot_x rotation)))
  (transform.rotate (euler_radians (* axis_rot_y rotation) 0 0))
  (if (input.get_button 'Shoot) (self.shoot))

  ; Cursor placing
  (local hit (raycast (transform.get_position) (transform.forward)))
  (if (non_null hit.collider)
    (do
      (cursor.require_primitive|.material|.vector 'color (vec 1 0 0 1))
      (cursor_transform.set_position hit.position)
    )
    (cursor.require_primitive|.material|.vector 'color (vec 0 0 0 0))
  )

  ; Rendering
  (if (input.get_button_pressed 'CelShading) (do
    (set cel_shading (not cel_shading))
    (entity_get "scene"|.require_group|.reload_level_script)
  ))

  ; Other actions
  (if (input.get_button_pressed 'Bach)
    (if (bach_source.is_playing)
      (bach_source.stop)
      (bach_source.play)))
  (if (input.get_button_pressed 'Guitar)
    (if (guitar_source.is_playing)
      (guitar_source.stop)
      (guitar_source.play)))
  (if (input.get_button_pressed 'Mozart)
    (if (mozart_source.is_playing)
      (mozart_source.stop)
      (mozart_source.play)))

  ; GUI
  (self.text_gui.material|.text
    (+ "FPS: " (/ 1.0 delta) "\n"
    "Frame: " avg_frame_work_duration "\n"))
)))
(set self.shoot (fn (do
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
