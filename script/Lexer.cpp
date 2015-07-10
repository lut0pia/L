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
    bool string(false);
    do {
      c = _stream.get();
      if(string) {
        *w++ = c;
        if(c=='"')
          break;
      } else {
        if(Stream::isspace(c)) { // End of word
          break;
        }  else if((c>='!' && c<='/') || (c>=':' && c<='@') || (c>='['&&c<='`') || c>='{') { // Special char
          if(w>_buffer) { // Word already started
            _stream.unget(c);
            break;
          } else {
            *w++ = c;
            if(c=='"') string = true; // Start of a string
            else break; // Single special char
          }
        } else
          *w++ = c;
      }
    } while(!_stream.eof());
    *w = '\0';
    return true;
  }
}
