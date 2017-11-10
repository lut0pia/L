#include "Midi.h"

#include "../dev/debug.h"
#include <windows.h>
#include <mmsystem.h>

using namespace L;
using namespace Audio;

static HMIDIOUT device(nullptr);

void Midi::send(const MidiEvent& e) {
  if(!device) {
    MMRESULT midiOutOpen_result(midiOutOpen(&device, 0, 0, 0, CALLBACK_NULL));
    L_ASSERT(midiOutOpen_result==MMSYSERR_NOERROR);
  }
  midiOutShortMsg(device, e.message);
}
