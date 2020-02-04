#include <L/src/audio/Midi.h>
#include <L/src/dev/debug.h>
#include <L/src/system/Memory.h>

#include <windows.h>
#include <mmsystem.h>

using namespace L;

class MidiWin : public Midi {
protected:
  HMIDIOUT _device;
public:
  MidiWin() {
    MMRESULT midiOutOpen_result(midiOutOpen(&_device, 0, 0, 0, CALLBACK_NULL));
    L_ASSERT(midiOutOpen_result==MMSYSERR_NOERROR);
  }
protected:
  void send_internal(const MidiEvent& e) override {
    midiOutShortMsg(_device, e.msg32);
  }
};

void midi_win_module_init() {
  Memory::new_type<MidiWin>();
}
