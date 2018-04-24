#pragma once

#include <cstdint> 

namespace L {
  namespace Script {
    class Lexer {
    private:
      char _buffer[256];
      const char* _read;
      size_t _read_size;
      char* _write;
      uint32_t _line;
      enum State : uint8_t {
        Started, Done, Comment,
      } _state;
      bool _literal : 1, _last_read : 1;

      char get();
      inline char peek() const { return *_read; }
      inline bool end_of_text() const { return _read_size==0; }
      inline static bool space_char(const char c) {
        return (c==' ' || c=='\t' || c=='\n' || c=='\v' || c=='\f' || c=='\r');
      }
      inline static bool special_char(const char c) {
        return (c=='(' || c==')' || c=='{' || c=='}' || c=='\'' || c=='"' || c=='!' || c=='|' || c==';');
      }

    public:
      inline Lexer() : _read(nullptr), _write(_buffer), _line(1), _state(Started), _literal(false), _last_read(false) {}
      //! Start reading new text
      //! @param text Should be available until reading is done, not copied
      //! @param size Char count of the text parameter
      //! @param last_read True if there is no more text after this one
      void read(const char* text, size_t size, bool last_read = false);
      //! Reads the next token
      //! @return true if a token has been formed, false if token is not complete yet
      bool next_token();
      //! Check and read next token
      //! Calls next_token if the current token was the one in the parameter
      //! @param token Token to check against
      //! @return true if the current token was accepted
      bool accept_token(const char* token);

      inline const char* token() const { return _buffer; }
      inline bool is_token(const char* str) { return !strcmp(str, token()); }
      inline uint32_t line() const { return _line; }
      inline bool literal() const { return _literal; }
    };
  }
}
