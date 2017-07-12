#include "Lexer.h"

#include <cstring>

using namespace L;
using namespace Script;

char Lexer::get() {
  char c(_peek);
  if(c=='\0')
    c = _stream.get();
  else
    _peek = '\0';
  if(c=='\n')
    _line++;
  return c;
}
char Lexer::peek() {
  if(_peek=='\0')
    _peek = _stream.get();
  return _peek;
}
bool Lexer::special_char(char c) {
  return (c=='(' || c==')' || c=='{' || c=='}' || c=='\'' || c=='"' || c=='!' || c=='|' || c==';');
}
bool Lexer::next_token() {
  char* w(_buffer);
  _literal = false;

  while(true) {
    if(w==_buffer) { // Before anything relevant
      char c(get());
      if(_stream.end()) { // End of stream
        _eos = true;
        return false;
      } else if(c==';') { // Comment
        do c = get();
        while(!_stream.end() && c!='\n');
      } else if(c=='"') { // Start of literal
        _literal = true;
      } else if(special_char(c)) { // One char token
        _buffer[0] = c;
        _buffer[1] = '\0';
        return true;
      } else if(!Stream::isspace(c)) { // Start of token
        *w++ = c;
      }
    } else if(_literal) { // Literal string
      char c(get());
      if(w>_buffer && *(w-1)=='\\') { // Escaped character
        switch(c) {
          case '\\':
            *(w-1) = '\\';
            break;
          case 't':
            *(w-1) = '\t';
            break;
          case 'n':
            *(w-1) = '\n';
            break;
          case 'r':
            *(w-1) = '\r';
            break;
          case '"':
            *(w-1) = '"';
            break;
          default:
            *w++ = c;
            break;
        }
      } else if(c=='"') { // End of literal string
        *w = '\0';
        return true;
      } else { // Simple character in string
        *w++ = c;
      }
    } else { // Regular token
      char c(peek());
      if(special_char(c) || Stream::isspace(c)) { // End of token
        *w = '\0';
        return true;
      } else {
        *w++ = get();
      }
    }
  }
}
bool Lexer::accept_token(const char* str) {
  if(is_token(str)) {
    next_token();
    return true;
  }
  return false;
}
