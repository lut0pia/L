#pragma once

#include "../container/StaticStack.h"
#include "Lexer.h"
#include "script.h"

namespace L {
  namespace Script {
    //! Compiles ls script into callable functions
    //! Should be thread-safe
    class Compiler {
    protected:
      Lexer _lexer;
      Var _code;
      StaticStack<128, Var*> _stack;
      enum {
        Usual, PostQuote,
      } _state;
    public:
      inline Compiler() { reset(); }
      //! Read new portion of text
      //! @param text Text to read tokens from
      //! @param size Char count of the text parameter
      //! @param last_read True if there is no more text after this one
      void read(const char* text, size_t size, bool last_read = false);
      void reset();
      CodeFunction function() const;

      inline bool ready() const { return _stack.size()==2; }

      //! TODO
      static void apply_scope(Var&, Table<Symbol, uint32_t>&, uint32_t&);
    };
  }
}
