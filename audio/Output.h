#ifndef DEF_L_Audio_Output
#define DEF_L_Audio_Output

#if defined L_WINDOWS
#include <windows.h>
#include <mmsystem.h>
#endif

#include "Buffer.h"
#include "../systems.h"
#include "../parallelism.h"
#include "../stl/List.h"

namespace L {
  namespace Audio {
    class Output {
      private:
        Format format;
        List<Ref<Buffer> > buffers;
        Mutex buffersMutex;
        Semaphore buffersSem;
        Thread buffersThread;
        //#if defined L_WINDOWS
        HWAVEOUT hwo;
        //#endif
      public:
        Output(const Format&);
        L_NoCopy(Output)
        ~Output();
        void write(const Ref<Buffer>&);
        Output& operator<<(const Ref<Buffer>&);

        static Dynamic::Var buffersFunc(Thread*);
    };
  }
}

#endif


