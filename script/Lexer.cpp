#include "Lexer.h"

#include <cstring>
#include "../String.h"
#include "../Exception.h"

using namespace L;
using namespace Script;

bool Lexer::nextToken() {
  if(_stream.end())
    return false;
  else {
    _stream.nospace();
    char* w(_buffer);
    char c;
    _literal = false;
    do {
      c = _stream.get();
      if(_literal) { // Literal expression
        if(c=='"') break; // End of string
        else *w++ = c; // Character inside string
      } else { // Non-literal token
        if(Stream::isspace(c)) break;  // End of word
        else if((c>='!' && c<='/') || (c>=':' && c<='@') || (c>='['&&c<='`') || c>='{') { // Special char
          if(w>_buffer) { // Word already started
            _stream.unget(c);
            break;
          } else {
            if(c=='"') _literal = true; // Start of a string
            else { // Single special char
              *w++ = c;
              break;
            }
          }
        } else *w++ = c; // Regular character
      }
    } while(!_stream.end()); // Reached end of stream
    *w = '\0'; // Null-ended
    return true;
  }
}

bool Lexer::isToken(const char* str) {
  return !strcmp(str,token());
}
bool Lexer::acceptToken(const char* str) {
  if(isToken(str)) {
    nextToken();
    return true;
  }
  return false;
}
void Lexer::expectToken(const char* str) {
  if(!acceptToken(str))
    throw Exception("Script: Expected "+String(str)+" but got "+String(token())+" instead.");
}
