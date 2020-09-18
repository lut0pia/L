(local m self)
(local face (fun p0 p1 p2 p3 n (do
  (m.push_position p0)
  (m.push_normal n)
  (m.push_position p1)
  (m.push_normal n)
  (m.push_position p2)
  (m.push_normal n)

  (m.push_position p0)
  (m.push_normal n)
  (m.push_position p2)
  (m.push_normal n)
  (m.push_position p3)
  (m.push_normal n)
)))

(face (vec -1 -1 -1) (vec -1 -1 1) (vec -1 1 1) (vec -1 1 -1) (vec -1 0 0)) ; Left
(face (vec 1 -1 -1) (vec 1 1 -1) (vec 1 1 1) (vec 1 -1 1) (vec 1 0 0)) ; Right
(face (vec -1 -1 -1) (vec 1 -1 -1) (vec 1 -1 1) (vec -1 -1 1) (vec 0 -1 0)) ; Back
(face (vec -1 1 -1) (vec -1 1 1) (vec 1 1 1) (vec 1 1 -1) (vec 0 1 0)) ; Front
(face (vec -1 -1 -1) (vec -1 1 -1) (vec 1 1 -1) (vec 1 -1 -1) (vec 0 0 -1)) ; Bottom
(face (vec -1 -1 1) (vec 1 -1 1) (vec 1 1 1) (vec -1 1 1) (vec 0 0 1)) ; Top
