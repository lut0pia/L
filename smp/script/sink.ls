; Big low collider that destroys everything to avoid ever falling garbage

(set self.start (fn (do
  (self.entity.require_transform|.move (vec 0 0 -128))
  (self.entity.require_collider|.box (vec 4096 4096 64))
)))

(set self.event (fn e (do
  (if (= e.type 'Collision) (entity_destroy (e.other.entity)))
)))
