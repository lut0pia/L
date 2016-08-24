#include "Lexer.h"

#include <cstring>
#include "../String.h"
#include "../macros.h"

using namespace L;
using namespace Script;

bool Lexer::nextToken() {
  _stream.nospace();
  while(!_stream.end() && _stream.peek()==';') { // Comment
    _stream.line(); // Ignore until end of line
    _stream.nospace();
  }
  if(_stream.end())
    return false;
  else {
    char* w(_buffer);
    _literal = false;
    do {
      char c(_stream.get());
      if(_literal) { // Literal expression
        if(*(w-1)=='\\')
          switch(c) {
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
          } else if(c=='"') break; // End of string
          else *w++ = c; // Character inside string
      } else { // Non-literal token
        if(Stream::isspace(c)) break;  // End of word
        else if(c==';'){ // Comment
          _stream.line();
          break;
        } else if(c=='(' || c==')' || c=='\'' || c=='"' || c=='!' || c=='|') { // Special char
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
    L_ERROR("Script: Expected "+String(str)+" but got "+String(token())+" instead.");
}
