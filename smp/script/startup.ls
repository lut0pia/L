(set rand_range (fn min max (+ min (* (- max min) (rand)))))
(set rand_color (fn (color (rand) (rand) (rand))))

(engine_gravity (vec 0 0 -9.8))
;(engine_gravity (vec 0 0 0))
;(engine_timescale 0.1)
(entity_make|.add_script|.load "script/camera.ls")
(entity_make "scene"|.add_group|.level_script "script/scene.ls")

(set mozart_source (entity_make|.require_midi_source))
(mozart_source.sequence "audio/mozart.mid")

(set guitar_source (entity_make|.require_audio_source))
(guitar_source.stream "audio/guitar.wav")

(set bach_source (entity_make|.require_audio_source))
(bach_source.stream "audio/bach.ogg")
