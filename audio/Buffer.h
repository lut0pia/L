#ifndef DEF_L_Audio_Buffer
#define DEF_L_Audio_Buffer

#include "Format.h"
#include "../macros.h"
#include "../containers/Array.h"
#include "../time.h"

namespace L {
  namespace Audio {
    class Buffer {
      private:
        Format format;
        Array<byte> bytes;
      public:
        Buffer(const Format&, size_t);
        Buffer(const Format&, const Time&);
        Buffer(const Format&, const Buffer&);

        inline const Format& gFormat() const {return format;}
        inline const byte* gBytes() const {return &bytes[0];}
        inline byte* gBytes() {return &bytes[0];}
        inline uint size() const {return bytes.size();}
    };
  }
}

#endif



