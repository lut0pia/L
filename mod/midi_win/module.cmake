add_module(
  midi_win
  CONDITION $<PLATFORM_ID:Windows>
  LIBRARIES Winmm
)
