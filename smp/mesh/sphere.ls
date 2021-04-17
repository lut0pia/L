(local m self)
(local div 4)
(local div2 (* div div))

(local push_pos (fn p (do
  (set p (normalize p))
  (m.push_position p)
  (m.push_normal p)
)))

(local subface (fn p0 p1 p2 p3 (do
  (push_pos p0)
  (push_pos p1)
  (push_pos p2)

  (push_pos p0)
  (push_pos p2)
  (push_pos p3)
)))

(local face (fn o xa ya (do
  (local i 0)
  (local xad (/ xa div))
  (local yad (/ ya div))
  (while (< i div2) (do
    (local x0i (floor (/ i div)))
    (local y0i (floor (% i div)))
    (local x1i (+ x0i 1))
    (local y1i (+ y0i 1))
    (local x0 (* x0i xad))
    (local y0 (* y0i yad))
    (local x1 (* x1i xad))
    (local y1 (* y1i yad))
    (subface (+ o x0 y0) (+ o x1 y0) (+ o x1 y1) (+ o x0 y1))
    (+= i 1)
  ))
)))

(face (vec -1 -1 -1) (vec 0 0 2) (vec 0 2 0)) ; Left
(face (vec 1 -1 -1) (vec 0 2 0) (vec 0 0 2)) ; Right
(face (vec -1 -1 -1) (vec 2 0 0) (vec 0 0 2)) ; Back
(face (vec -1 1 -1) (vec 0 0 2) (vec 2 0 0)) ; Front
(face (vec -1 -1 -1) (vec 0 2 0) (vec 2 0 0)) ; Bottom
(face (vec -1 -1 1) (vec 2 0 0) (vec 0 2 0)) ; Top
