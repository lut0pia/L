#pragma once

#include <L/src/container/Array.h>
#include <L/src/container/StaticStack.h>
#include <L/src/container/Table.h>
#include <L/src/dynamic/Variable.h>
#include "LSLexer.h"

//! Parses ls script into an abstract syntax tree
//! Should be thread-safe
class LSParser {
public:
  enum class NodeType : uint8_t {
    Value,
    Array,
    Access,
    Raw,
  };
  struct Node {
    inline Node() {}
    inline Node(const L::Array<Node>& c) : children(c), type(NodeType::Array) {}
    inline Node(const L::Var& v) : value(v), type(NodeType::Value) {}
    L::Array<Node> children;
    L::Var value;
    uint32_t line = 0;
    NodeType type = NodeType::Value;
    bool dot_access = false;
  };
protected:
  LSLexer _lexer;
  Node _ast;
  L::StaticStack<128, Node*> _stack;
  L::Table<L::Var*, uint32_t> _lines;
public:
  inline LSParser() { reset(); }
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
  const Node& finish();
};
