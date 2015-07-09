#ifndef DEF_L_Script_Lexer
#define DEF_L_Script_Lexer

#include "../streams/Stream.h"

namespace L {
  namespace Script {
    class Lexer {
      private:
        Stream& _stream;
        char _buffer[1024];
      public:
        inline Lexer(Stream& stream) : _stream(stream) {}
        bool nextToken();
        inline const char* token() const {return _buffer;}
    };
  }
}

#endif

