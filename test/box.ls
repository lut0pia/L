(local event (fun (e) (do
  (if (= (e'type) 'COLLISION)
    (if (= (e'other) character-collider)
      (if (< (rand) 0.5) (entity-destroy entity))
    )
  )
)))
