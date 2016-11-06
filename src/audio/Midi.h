#pragma once

/*  Windows libraries :
**      - midiio
**      - winmm
*/

#include "../types.h"

#if defined L_WINDOWS
# include <windows.h>
# include <mmsystem.h>
#endif

#define L_ToMidiNote(n) ((n)+24) // Shift the note to the right midi equivalent

namespace L {
  namespace Audio {
    class Midi {
      private:
#if defined L_WINDOWS
        static HMIDIOUT device;
#elif defined L_UNIX
        static int fd;
#endif

        static void send(byte msgData[4]);

      public:
        static void sInstrument(byte channel, byte instrument);
        static void playNote(byte channel, byte noteNumber, byte velocity=127);
        static void stopNote(byte channel, byte noteNumber);
        static void stopAll(byte channel);
        static void stopAll();
    };
  }
}
