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
public:
  inline LSParser() { reset(); } // Allows
  //! Read new portion of text
  //! @param context Small debug string to give context to warnings
  //! @param text Text to read tokens from
  //! @param size Char count of the text parameter
  //! @return true if successful
  bool read(const char* context, const char* text, size_t size);
  //! Reset state of the parser
  void reset();
  //! Get AST and reset parser
  //! @return AST
  L::Var finish();
};
