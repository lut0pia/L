(input_map [
  ; Gamepad movenent
  {
    'name 'XAxis
    'axis 'GamepadLeftStickX
  }
  {
    'name 'YAxis
    'axis 'GamepadLeftStickY
  }

  ; Gamepad rotation
  {
    'name 'XAxisRot
    'axis 'GamepadRightStickX
    'multiplier -1
  }
  {
    'name 'YAxisRot
    'axis 'GamepadRightStickY
  }

  ; Gamepad shoot
  {
    'name 'Shoot
    'button 'GamepadFaceBottom
  }

  ; Keyboard movememt
  {
    'name 'XAxis
    'button 'A ; Left
    'multiplier -1
  }
  {
    'name 'XAxis
    'button 'Q ; Left
    'multiplier -1
  }
  {
    'name 'XAxis
    'button 'D ; Right
  }
  {
    'name 'YAxis
    'button 'W ; Forward
  }
  {
    'name 'YAxis
    'button 'Z ; Forward
  }
  {
    'name 'YAxis
    'button 'S ; Backward
    'multiplier -1
  }

  ; Keyboard slow
  {
    'name 'Slow
    'button 'LeftShift
  }

  ; Mouse rotation
  {
    'name 'XAxisRot
    'axis 'MouseDX
    'multiplier -0.1
  }
  {
    'name 'YAxisRot
    'axis 'MouseDY
    'multiplier -0.1
  }

  ; Mouse shoot
  {
    'name 'Shoot
    'button 'MouseLeft
  }

  ; Keyboard misc
  {
    'name 'Restart
    'button 'R
  }
  {
    'name 'Bach
    'button 'B
  }
  {
    'name 'Guitar
    'button 'G
  }
  {
    'name 'Mozart
    'button 'M
  }
])
