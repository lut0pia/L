(local event (fun (e) (do
  (if (= (e'type) 'COLLISION)
    (if (= (e'other) character-rigidbody)
      (if (< (rand) 0.5) (entity-destroy entity))
    )
  )
)))
