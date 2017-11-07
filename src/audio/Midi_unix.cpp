#include "Midi.h"

using namespace L;
using namespace Audio;

void Midi::send(const MidiEvent& e) {
  static int midifd(open("/dev/midi", O_WRONLY,0));
  write(midifd, &e, sizeof(e));
}
