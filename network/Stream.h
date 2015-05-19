#ifndef DEF_L_Network_Stream
#define DEF_L_Network_Stream

#include <iostream>
#include "Network.h"

namespace L {
  namespace Network {
    class StreamBuf : public std::streambuf {
      private:
        SOCKET sd;
        char buffer;
        bool buffered;
      public:
        StreamBuf(SOCKET sd);
        // Input
        std::streamsize xsgetn(char*, std::streamsize);
        int underflow();
        int uflow();
        // Output
        std::streamsize xsputn(const char* s, std::streamsize n);
        int overflow(int = EOF);
    };
    class Stream : public std::iostream {
      private:
        StreamBuf* sb;
      public:
        Stream(SOCKET);
        virtual ~Stream();
    };
  }
}

#endif

