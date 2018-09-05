#pragma once

#include <L/src/container/StaticStack.h>
#include <L/src/script/script.h>
#include "LSLexer.h"

//! Compiles ls script into callable functions
//! Should be thread-safe
class LSCompiler {
protected:
  LSLexer _lexer;
  L::Var _code;
  L::StaticStack<128, L::Var*> _stack;
  enum {
    Usual, PostQuote,
  } _state;
public:
  inline LSCompiler() { reset(); }
  //! Read new portion of text
  //! @param text Text to read tokens from
  //! @param size Char count of the text parameter
  //! @param last_read True if there is no more text after this one
  void read(const char* text, size_t size, bool last_read = false);
  void reset();
  L::Script::CodeFunction function() const;

  inline bool ready() const { return _stack.size()==2; }

  static void apply_scope(L::Var&, L::Table<L::Symbol, uint32_t>&, uint32_t&);
};
