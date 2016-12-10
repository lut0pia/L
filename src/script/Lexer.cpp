#include "Lexer.h"

#include <cstring>
#include "../text/String.h"
#include "../macros.h"

using namespace L;
using namespace Script;

char Lexer::get(){
  char c(_stream.get());
  if(c=='\n') _line++;
  return c;
}
void Lexer::nospace() {
  char c;
  do c = get();
  while(Stream::isspace(c));
  _stream.unget(c);
}
void Lexer::comment() {
  char c;
  do c = get();
  while(c!='\n');
  nospace();
}
bool Lexer::nextToken() {
  nospace();
  while(!_stream.end() && _stream.peek()==';')
    comment();
  if(_stream.end()){
    _eos = true;
    return false;
  } else {
    char* w(_buffer);
    _literal = false;
    do {
      char c(get());
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
          comment();
          break;
        } else if(c=='(' || c==')' || c=='{' || c=='}' || c=='\'' || c=='"' || c=='!' || c=='|') { // Special char
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
bool Lexer::acceptToken(const char* str) {
  if(isToken(str)) {
    nextToken();
    return true;
  }
  return false;
}
void Lexer::expectToken(const char* str) {
  if(!acceptToken(str))
    L_ERRORF("Script: Expected token %s but got %s instead on line %d.",str,_buffer,_line);
}
