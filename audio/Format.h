#ifndef DEF_L_Audio_Format
#define DEF_L_Audio_Format

#include "../macros.h"
#include "../time.h"
#if defined L_WINDOWS
    #include <windows.h>
    #include <mmsystem.h>
#endif

namespace L{
    namespace Audio{
        class Format{
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



