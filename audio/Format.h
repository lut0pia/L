#ifndef DEF_L_Audio_Format
#define DEF_L_Audio_Format

#include "../systems.h"
#include "../time.h"

namespace L {
  namespace Audio {
    class Format {
      private:
        ushort channels, samplingRate, bytesPerSample;
      public:
        Format(ushort channels, ushort samplingRate, ushort bytesPerSample);
        bool operator==(const Format&) const;
        bool operator!=(const Format&) const;

        void display() const;

        uint bytesPerSecond() const;
#if defined L_WINDOWS
        WAVEFORMATEX gWaveFormatEx() const;
#endif
    };
  }
}

#endif



