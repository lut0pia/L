#include "LSParser.h"

#include "ls.h"

using namespace L;

static const Symbol object_symbol("object"), do_symbol("do");

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
    L_ASSERT(_stack.top(1)->is<Array<Var>>());
    Array<Var>& top_array = _stack.top(1)->as<Array<Var>>();
    if(_lexer.is_token("(")) { // It's a list of expressions
      _stack.top()->make<Array<Var>>();
    } else if(_lexer.is_token("{")) {
      _stack.top()->make<Array<Var>>().push(object_symbol);
    } else if(_lexer.is_token(")") || _lexer.is_token("}")) {
      if(_stack.size() <= 2) {
        warning("ls: %s:%d: Unexpected token '%s'", context, _lexer.line(), _lexer.token());
        return false;
      }
      top_array.pop();
      _stack.pop();
      _stack.pop();
    } else if(_lexer.is_token(".") || _lexer.is_token(":")) {
      top_array.pop(); // Remove current value
      Var& previous_var(top_array.back());
      if(previous_var.is<AccessChain>()) {
        previous_var.as<AccessChain>().array.push();
      } else {
        previous_var = AccessChain {Array<Var> {previous_var, Var()}};
      }
      // Remember last access type as char (.:[)
      previous_var.as<AccessChain>().last_access_type = *_lexer.token();
      _stack.top() = previous_var.as<AccessChain>().array.end() - 1;
      continue;
    } else if(_lexer.is_token("|")) {
      if(!_stack.top(1)->is<Array<Var>>()) {
        warning("ls: %s:%d: Invalid ' after '", context, _lexer.line());
        return false;
      }
      top_array.pop(); // Remove current value
      top_array = Array<Var> {top_array,Var()};
      _stack.top() = &top_array[1];
    } else if(_lexer.is_token("'")) {
      if(!_lexer.next_token()) {
        warning("ls: %s:%d: Unexpected end of file after '", context, _lexer.line());
        return false;
      }
      if(valid_symbol(_lexer.token())) {
        _stack.top()->make<RawSymbol>().sym = _lexer.token();
      } else {
        warning("ls: %s:%d: Using ' before non-symbol '%s'", context, _lexer.line(), _lexer.token());
        return false;
      }
      _stack.pop();
    } else {
      const char* token(_lexer.token());
      Var& v(*_stack.top());
      if(_lexer.literal()) v = token; // Character string
      else if(strpbrk(token, "0123456789") && token[strspn(token, "-0123456789.")] == '\0') v = float(atof(token));
      else if(_lexer.is_token("true")) v = true;
      else if(_lexer.is_token("false")) v = false;
      else v = Symbol(token);
      if(AccessChain* access_chain = top_array.back().try_as<AccessChain>()) {
        if(access_chain->last_access_type == '.' && v.is<Symbol>()) {
          const Symbol sym(v.as<Symbol>());
          v.make<RawSymbol>().sym = sym;
        }
      }
      _stack.pop();
    }
    if(_stack.top()->is<Array<Var>>()) {
      _stack.top()->as<Array<Var>>().push();
      _stack.push(&_stack.top()->as<Array<Var>>().back());
    }
  }
  if(_stack.size() != 2) {
    warning("ls: %s: Unexpected end of file", context);
    return false;
  }
  return true;
}
void LSParser::reset() {
  Array<Var>& var_array(_ast.make<Array<Var>>());
  var_array.push(do_symbol);
  var_array.push(); // Reading always happens on a void element

  // Setup stack
  _stack.clear();
  _stack.push(&_ast);
  _stack.push(&var_array.back());
}
const Var& LSParser::finish() {
  // Can only finish if the stack size is 2
  // Which means the first element is the do sequence
  // and the second is a void element we are going
  // to remove before returning
  L_ASSERT(_stack.size() == 2);

  // Remove void element
  _ast.as<Array<Var>>().pop();

  _stack.clear();

  return _ast;
}