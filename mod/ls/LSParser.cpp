#include "LSParser.h"

#include <cstdlib>

using namespace L;

static const Symbol object_symbol("{object}"), array_symbol("[array]"), do_symbol("do");

static bool valid_symbol(const char* token) {
  while(*token) {
    if((*token >= 'A' && *token <= 'Z') || (*token >= 'a' && *token <= 'z') ||
      (*token >= '0' && *token <= '9') || *token == '_') {
      token++;
    } else {
      return false;
    }
  }

  return true;
}

bool LSParser::read(const char* context, const char* text, size_t size) {
  _lexer.read(text, size, true);
  while(_lexer.next_token()) {
    L_ASSERT(_stack.top(1)->type == NodeType::Array || _stack.top(1)->type == NodeType::Access);
    _stack.top()->line = _lexer.line();
    Array<Node>& top_array = _stack.top(1)->children;
    if(_lexer.is_token("(")) { // It's a list of expressions
      _stack.top()->type = NodeType::Array;
    } else if(_lexer.is_token("{")) {
      _stack.top()->type = NodeType::Array;
      _stack.top()->children.push(object_symbol);
    } else if(_lexer.is_token("[")) {
      _stack.top()->type = NodeType::Array;
      _stack.top()->children.push(array_symbol);
    } else if(_lexer.is_token(")") || _lexer.is_token("}") || _lexer.is_token("]")) {
      if(_stack.size() <= 2) {
        warning("ls: %s:%d: Unexpected token '%s'", context, _lexer.line(), _lexer.token());
        return false;
      }
      const char* expected_token = ")";
      if(top_array[0].value == object_symbol) {
        expected_token = "}";
      } else if(top_array[0].value == array_symbol) {
        expected_token = "]";
      } 
      if(!_lexer.is_token(expected_token)) {
        warning("ls: %s:%d: Unexpected token '%s', was expecting '%s'", context, _lexer.line(), _lexer.token(), expected_token);
        return false;
      }
      if(top_array.size() == 1) {
        warning("ls: %s:%d: Invalid '()' statement", context, _lexer.line());
        return false;
      }
      top_array.pop();
      _stack.pop();
      _stack.pop();
    } else if(_lexer.is_token(".") || _lexer.is_token(":")) {
      top_array.pop(); // Remove current value
      Node& previous_node = top_array.back();
      if(previous_node.type == NodeType::Access) {
        previous_node.children.push();
      } else {
        previous_node.line = _lexer.line();
        previous_node = Array<Node> {previous_node, Var()};
        previous_node.line = _lexer.line();
        previous_node.type = NodeType::Access;
      }
      previous_node.dot_access = *_lexer.token() == '.';
      _stack.top() = previous_node.children.end() - 1;
      continue;
    } else if(_lexer.is_token("|")) {
      if(_stack.top(1)->type != NodeType::Array) {
        warning("ls: %s:%d: Invalid ' after '", context, _lexer.line());
        return false;
      }
      const uint32_t line = top_array.back().line;
      top_array.pop(); // Remove current value
      top_array = Array<Node> {top_array, Var()};
      top_array[0].line = line;
      _stack.top() = &top_array[1];
    } else if(_lexer.is_token("'")) {
      if(!_lexer.next_token()) {
        warning("ls: %s:%d: Unexpected end of file after '", context, _lexer.line());
        return false;
      }
      if(valid_symbol(_lexer.token())) {
        _stack.top()->type = NodeType::Raw;
        _stack.top()->line = _lexer.line();
        _stack.top()->value = Symbol(_lexer.token());
      } else {
        warning("ls: %s:%d: Using ' before non-symbol '%s'", context, _lexer.line(), _lexer.token());
        return false;
      }
      _stack.pop();
    } else {
      const char* token = _lexer.token();
      Var& v = _stack.top()->value;
      if(_lexer.literal()) v = token; // Character string
      else if(strpbrk(token, "0123456789") && token[strspn(token, "-0123456789.")] == '\0') v = float(atof(token));
      else if(_lexer.is_token("true")) v = true;
      else if(_lexer.is_token("false")) v = false;
      else v = Symbol(token);
      if(top_array.back().type == NodeType::Access && top_array.back().dot_access && v.is<Symbol>()) {
        _stack.top()->type = NodeType::Raw;
      }
      _stack.top()->line = _lexer.line();
      _stack.pop();
    }
    if(_stack.top()->type == NodeType::Array) {
      _stack.top()->children.push();
      _stack.top()->line = _lexer.line();
      _stack.push(&_stack.top()->children.back());
    }
  }
  if(_stack.size() != 2) {
    warning("ls: %s: Unexpected end of file", context);
    return false;
  }
  return true;
}
void LSParser::reset() {
  _ast.children.clear();
  _ast.type = NodeType::Array;
  _ast.children.push(do_symbol);
  _ast.children.push(); // Reading always happens on a void element

  // Setup stack
  _stack.clear();
  _stack.push(&_ast);
  _stack.push(&_ast.children.back());
}
const LSParser::Node& LSParser::finish() {
  // Can only finish if the stack size is 2
  // Which means the first element is the do sequence
  // and the second is a void element we are going
  // to remove before returning
  L_ASSERT(_stack.size() == 2);

  // Remove void element
  _ast.children.pop();

  _stack.clear();

  return _ast;
}
