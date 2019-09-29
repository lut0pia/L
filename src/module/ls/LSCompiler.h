#pragma once

#include <L/src/container/Array.h>
#include <L/src/container/StaticStack.h>
#include <L/src/container/Table.h>
#include <L/src/engine/Resource.h>
#include <L/src/script/script.h>
#include <L/src/text/Symbol.h>
#include "LSParser.h"

//! Compiles ls script into callable functions
//! Should be thread-safe
class LSCompiler {
protected:
  LSParser _parser;
  struct Function {
    Function* parent;
    L::Table<L::Symbol, uint8_t> local_table;
    L::Array<L::Symbol> outers;
    L::Var code;
    L::Array<L::ScriptInstruction> bytecode;
    uint8_t local_count;
    uint32_t bytecode_offset;
  };
  L::Array<Function*> _functions;
  L::Ref<L::Script> _script;

public:
  //! Read new portion of text
  //! @param text Text to read tokens from
  //! @param size Char count of the text parameter
  //! @param last_read True if there is no more text after this one
  bool read(const char* text, size_t size, bool last_read = false);

  //! Compile function from everything parsed earlier
  //! Resets parser state
  L::ScriptFunction compile();

protected:
  Function& make_function(const L::Var& code, Function* parent = nullptr);
  bool find_outer(Function&, const L::Symbol& symbol, uint8_t& outer);
  void resolve_locals(Function&, const L::Var&);
  void compile(Function&, const L::Var&, uint8_t offset = 0);
  void compile_function(Function&);
  //! Puts object at offset+1 and last index at offset
  //! If get is true, does the final access to put the value at offset
  void compile_access_chain(Function&, const L::Array<L::Var>& array, uint8_t offset, bool get);
  void compile_function_call(Function& func, const L::Array<L::Var>& array, uint8_t offset);
  void compile_assignment(Function& func, const L::Var& dst, uint8_t src, uint8_t offset);
  void compile_operator(Function& func, const L::Array<L::Var>& array, uint8_t offset, L::ScriptOpCode opcode);
  void compile_op_assign(Function& func, const L::Array<L::Var>& array, uint8_t offset, L::ScriptOpCode opcode);
  void compile_comparison(Function& func, const L::Array<L::Var>& array, uint8_t offset, L::ScriptOpCode cmp, bool not = false);
};
