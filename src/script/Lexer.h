#pragma once

#include "../stream/Stream.h"

namespace L {
  namespace Script {
    class Lexer {
    private:
      Stream& _stream;
      char _buffer[1024];
      uint32_t _line;
      char _peek;
      bool _literal : 1;
      bool _eos : 1;

      char get();
      char peek();
      static bool special_char(char);

    public:
      inline Lexer(Stream& stream) : _stream(stream), _line(1), _peek('\0'), _eos(false) {}
      bool next_token();
      inline bool is_token(const char* str) { return !strcmp(str, token()); }
      bool accept_token(const char*);

      inline const char* token() const { return _buffer; }
      inline uint32_t line() const { return _line; }
      inline bool literal() const { return _literal; }
      inline bool eos() const { return _eos; }
    };
  }
}
