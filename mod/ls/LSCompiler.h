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
    LSParser::Node code;
    L::Array<L::ScriptInstruction> bytecode;
    L::Array<uint32_t> bytecode_line;
    uint8_t local_count;
    uint32_t bytecode_offset;

    inline void push(const LSParser::Node& node, L::ScriptInstruction instruction) {
      L_ASSERT(node.line > 0);
      bytecode.push(instruction);
      bytecode_line.push(node.line);
    }
  };
  L::Array<Function*> _functions;
  L::Ref<L::Script> _script;
  L::Array<L::String> _source_lines;
  L::String _context;

public:
  //! Read new portion of text
  //! @param text Text to read tokens from
  //! @param size Char count of the text parameter
  //! @return true if successful
  bool read(const char* text, size_t size);

  //! Compile function from everything parsed earlier
  //! Resets parser state
  bool compile(L::ScriptFunction&);

  //! Sets context debug string to give context to warnings
  inline void set_context(const char* context) { _context = context; }

protected:
  Function& make_function(const LSParser::Node& code, Function* parent = nullptr);
  bool find_outer(Function&, const L::Symbol& symbol, uint8_t& outer);
  void resolve_locals(Function&, const LSParser::Node& code);
  bool compile(Function&, const LSParser::Node&, uint8_t offset = 0);
  bool compile_function(Function&);
  //! Puts object at offset+1 and last index at offset
  //! If get is true, does the final access to put the value at offset
  void compile_access_chain(Function&, const L::Array<LSParser::Node>& array, uint8_t offset, bool get);
  void compile_function_call(Function& func, const L::Array<LSParser::Node>& array, uint8_t offset);
  void compile_assignment(Function& func, const LSParser::Node& dst, uint8_t src, uint8_t offset);
  void compile_operator(Function& func, const L::Array<LSParser::Node>& array, uint8_t offset, L::ScriptOpCode opcode);
  void compile_op_assign(Function& func, const L::Array<LSParser::Node>& array, uint8_t offset, L::ScriptOpCode opcode);
  void compile_comparison(Function& func, const L::Array<LSParser::Node>& array, uint8_t offset, L::ScriptOpCode cmp, bool not = false);
};
