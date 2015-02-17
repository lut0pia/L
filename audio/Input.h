#ifndef DEF_L_Audio_Input
#define DEF_L_Audio_Input

#include "Buffer.h"
#include "../containers/Ref.h"
#include "../macros.h"
#if defined L_WINDOWS
    #include <windows.h>
    #include <mmsystem.h>
#endif

namespace L{
    namespace Audio{
        class Input{
            private:
                Format format;
                //#if defined L_WINDOWS
                    HWAVEIN hwi;
                //#endif
            public:
                Input(const Format&);
                L_NoCopy(Input)
                ~Input();
                const Format& gFormat() const{return format;}
                void read(const Ref<Buffer>&);

        };
    }
}

#endif


