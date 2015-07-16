#ifndef DEF_L_Script_Lexer
#define DEF_L_Script_Lexer

#include "../streams/Stream.h"

namespace L {
  namespace Script {
    class Lexer {
      private:
        Stream& _stream;
        char _buffer[1024];
        bool _literal;

      public:
        inline Lexer(Stream& stream) : _stream(stream) {}
        bool nextToken();
        bool isToken(const char*);
        bool acceptToken(const char*);
        void expectToken(const char*);

        inline const char* token() const {return _buffer;}
        inline bool literal() const {return _literal;}
    };
  }
}

#endif

