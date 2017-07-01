#pragma once

#include "../stream/Stream.h"

namespace L {
  namespace Script {
    class Lexer {
    private:
      Stream& _stream;
      char _buffer[1024];
      uint32_t _line;
      bool _literal : 1;
      bool _eos : 1;

      char get();
      void nospace();
      void comment();

    public:
      inline Lexer(Stream& stream) : _stream(stream),_line(1),_eos(false) {}
      bool nextToken();
      inline bool isToken(const char* str) { return !strcmp(str,token()); }
      bool acceptToken(const char*);
      void expectToken(const char*);

      inline const char* token() const { return _buffer; }
      inline uint32_t line() const{ return _line; }
      inline bool literal() const { return _literal; }
      inline bool eos() const { return _eos; }
    };
  }
}
