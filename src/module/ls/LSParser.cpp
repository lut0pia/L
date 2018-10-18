#include "LSParser.h"

#include "ls.h"

using namespace L;

static const Symbol object_symbol("object"), do_symbol("do");

bool LSParser::read(const char* text, size_t size, bool last_read) {
  _lexer.read(text, size, last_read);
  while(_lexer.next_token()) {
    if(_lexer.is_token("(")) { // It's a list of expressions
      _stack.top()->make<Array<Var>>();
    } else if(_lexer.is_token("{")) {
      _stack.top()->make<Array<Var>>().push(object_symbol);
    } else if(_lexer.is_token(")") || _lexer.is_token("}")) {
      if(_stack.size()<=2) {
        warning("Unexpected token '%s'", _lexer.token());
      }
      L_ASSERT(_stack.top(1)->is<Array<Var>>());
      _stack.top(1)->as<Array<Var>>().pop();
      _stack.pop();
      _stack.pop();
    } else if(_lexer.is_token(".") || _lexer.is_token(":")) {
      L_ASSERT(_state==Usual);
      L_ASSERT(_stack.top(1)->is<Array<Var>>());
      auto& var_array = _stack.top(1)->as<Array<Var>>();
      var_array.pop(); // Remove current value
      Var& previous_var(var_array.end()[-1]);
      if(previous_var.is<AccessChain>()) {
        previous_var.as<AccessChain>().array.push();
      } else {
        previous_var = AccessChain {Array<Var> {previous_var, Var()}};
      }
      // Remember last access type as char (.:[)
      previous_var.as<AccessChain>().last_access_type = *_lexer.token();
      _stack.top() = previous_var.as<AccessChain>().array.end()-1;
      continue;
    } else if(_lexer.is_token("|")) {
      L_ASSERT(_stack.top(1)->is<Array<Var>>());
      auto& var_array = _stack.top(1)->as<Array<Var>>();
      var_array.pop(); // Remove current value
      var_array = Array<Var> {var_array,Var()};
      _stack.top() = &var_array[1];
    } else if(_lexer.is_token("'")) {
      L_ASSERT(_state==Usual);
      _state = PostQuote;
      continue;
    } else {
      const char* token(_lexer.token());
      Var& v(*_stack.top());
      if(_lexer.literal()) v = token; // Character string
      else if(strpbrk(token, "0123456789") && token[strspn(token, "-0123456789.")]=='\0') v = float(atof(token));
      else if(_lexer.is_token("true")) v = true;
      else if(_lexer.is_token("false")) v = false;
      else v = Symbol(token);
      if (_state == PostQuote) {
        if(v.is<Symbol>()) {
          const Symbol sym(v.as<Symbol>());
          v.make<RawSymbol>().sym = sym;
        } else {
          error("Using ' before non-symbol");
        }
        _state = Usual;
      } else if(AccessChain* access_chain = _stack.top(1)->as<Array<Var>>().back().try_as<AccessChain>()) {
        if(access_chain->last_access_type=='.' && v.is<Symbol>()) {
          const Symbol sym(v.as<Symbol>());
          v.make<RawSymbol>().sym = sym;
        }
      }
      _stack.pop();
    }
    L_ASSERT(_state==Usual);
    if(_stack.top()->is<Array<Var>>()) {
      _stack.top()->as<Array<Var>>().push();
      _stack.push(&_stack.top()->as<Array<Var>>().back());
    }
  }
  return _stack.size()==2;
}
void LSParser::reset() {
  Array<Var>& var_array(_ast.make<Array<Var>>());
  var_array.push(do_symbol);
  var_array.push(); // Reading always happens on a void element

  // Setup stack
  _stack.clear();
  _stack.push(&_ast);
  _stack.push(&var_array.back());

  _state = Usual;
}
Var LSParser::finish() {
  // Can only finish if the stack size is 2
  // Which means the first element is the do sequence
  // and the second is a void element we are going
  // to remove before returning
  L_ASSERT(_stack.size()==2);

  // Remove void element
  _ast.as<Array<Var>>().pop();

  // Make a copy of the ast before resetting
  const Var sanitized_ast(_ast);

  reset();

  return sanitized_ast;
}