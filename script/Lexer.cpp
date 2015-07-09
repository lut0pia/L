#include "Lexer.h"

using namespace L;
using namespace Script;

bool Lexer::nextToken() {
  _stream.nospace();
  if(_stream.eof())
    return false;
  else {
    char* w(_buffer);
    char c;
    do {
      c = _stream.get();
      if(Stream::isspace(c)) { // End of word
        *w = '\0';
        return true;
      } else if((c>='!' && c<='/') || (c>=':' && c<='@') || (c>='['&&c<='`') || c>='{') { // Special char
        if(w>_buffer) { // Word already started
          *w = '\0';
          _stream.unget(c);
          return true;
        } else {
          _buffer[0] = c;
          _buffer[1] = '\0';
          return true;
        }
      } else
        *w++ = c;
    } while(!_stream.eof());
    *w = '\0';
    return true;
  }
}
