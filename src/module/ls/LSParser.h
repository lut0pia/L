#pragma once

#include <L/src/container/StaticStack.h>
#include <L/src/dynamic/Variable.h>
#include "LSLexer.h"

//! Parses ls script into an abstract syntax tree
//! Should be thread-safe
class LSParser {
protected:
  LSLexer _lexer;
  L::Var _ast;
  L::StaticStack<128, L::Var*> _stack;
  enum {
    Usual, PostQuote, Parsed,
  } _state;
public:
  inline LSParser() { reset(); } // Allows
  //! Read new portion of text
  //! @param text Text to read tokens from
  //! @param size Char count of the text parameter
  //! @param last_read True if there is no more text after this one
  //! @return true if AST is ready
  bool read(const char* text, size_t size, bool last_read = false);
  //! Reset state of the parser
  void reset();
  //! Get AST and reset parser
  //! @return AST
  L::Var finish();
};
