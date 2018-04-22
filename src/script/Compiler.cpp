#include "Compiler.h"

using namespace L;
using namespace Script;

void Compiler::read(const char* text, size_t size, bool last_read) {
  _lexer.read(text, size, last_read);
  while(_lexer.next_token()) {
    if(_lexer.is_token("(")) { // It's a list of expressions
      _stack.top()->make<Array<Var>>();
    } else if(_lexer.is_token("{")) {
      _stack.top()->make<Array<Var>>().push(Symbol("object"));
    } else if(_lexer.is_token(")") || _lexer.is_token("}")) {
      if(_stack.size()<=2) {
        warning("Unexpected token '%s'", _lexer.token());
      }
      L_ASSERT(_stack.top(1)->is<Array<Var>>());
      _stack.top(1)->as<Array<Var>>().pop();
      _stack.pop();
      _stack.pop();
    } else if(_lexer.is_token("|")) {
      L_ASSERT(_stack.top(1)->is<Array<Var>>());
      auto& var_array = _stack.top(1)->as<Array<Var>>();
      var_array.pop(); // Remove current value
      var_array = Array<Var>{var_array,Var()};
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
      if(_state == PostQuote) {
        L_ASSERT(v.is<Symbol>());
        const Symbol sym(v.as<Symbol>());
        v.make<RawSymbol>().sym = sym;
        _state = Usual;
      }
      _stack.pop();
    }
    L_ASSERT(_state==Usual);
    if(_stack.top()->is<Array<Var>>()) {
      _stack.top()->as<Array<Var>>().push();
      _stack.push(&_stack.top()->as<Array<Var>>().back());
    }
  }
}
void Compiler::reset() {
  static const Symbol do_symbol("do");
  Array<Var>& var_array(_code.make<Array<Var>>());
  var_array.push(do_symbol);
  var_array.push(); // Reading always happens on a void element

  // Setup stack
  _stack.clear();
  _stack.push(&_code);
  _stack.push(&var_array.back());

  _state = Usual;
}
CodeFunction Compiler::function() const {
  L_ASSERT(ready());
  CodeFunction code_function{_code, 0};
  code_function.code.as<Array<Var>>().pop(); // Remove temporary void value
  Table<Symbol, uint32_t> local_table;
  apply_scope(code_function.code, local_table, code_function.localCount);
  return code_function;
}

void Compiler::apply_scope(Var& v, Table<Symbol, uint32_t>& localTable, uint32_t& localIndex) {
  static const Symbol localSymbol("local"), funSymbol("fun"), foreachSymbol("foreach"), setSymbol("set");
  if(v.is<Array<Var>>()) {
    Array<Var>& array(v.as<Array<Var>>());
    if(array[0].is<Symbol>()) {
      const Symbol& sym(array[0].as<Symbol>());
      if(array[1].is<Symbol>() && sym==localSymbol) {
        array[0] = setSymbol;
        localTable[array[1].as<Symbol>()] = localIndex++;
      } else if(sym==foreachSymbol) {
        if(array.size()>=4) // Value only
          localTable[array[1].as<Symbol>()] = localIndex++;
        if(array.size()>=5) // Key value
          localTable[array[2].as<Symbol>()] = localIndex++;
      } else if(sym==funSymbol) // Don't go inside functions
        return;
    }
    for(Var& e : array)
      apply_scope(e, localTable, localIndex);
  } else if(v.is<Symbol>()) {
    if(uint32_t* found = localTable.find(v.as<Symbol>()))
      v = Local{*found};
  }
}

/*
bool Compiler::read(Var& v) {
  if(_lexer.eos()) {
    warning("Unexpected end of stream while parsing script: there are uneven () or {}.");
    return false;
  }
  if(_lexer.accept_token("(")) { // It's a list of expressions
    v.make<Array<Var> >();
    int i(0);
    while(!_lexer.accept_token(")"))
      if(_lexer.accept_token("|"))
        v = Array<Var>{v}, i = 1;
      else if(!read(v[i++], _lexer))
        return false;
  } else if(_lexer.accept_token("{")) { // It's an object
    v.make<Array<Var> >();
    v[0] = (Function)object;
    int i(1);
    while(!_lexer.accept_token("}"))
      if(!read(v[i++], _lexer))
        return false;
  } else if(_lexer.accept_token("'")) {
    if(!read(v, _lexer))
      return false;
    if(!v.is<Symbol>()) {
      warning("Expected symbol after ', got %s at line %d", (const char*)v.type()->name, _lexer.line());
      return false;
    }
    const Symbol sym(v.as<Symbol>());
    v.make<RawSymbol>().sym = sym;
  } else if(_lexer.accept_token("!")) {
    if(!read(v, _lexer))
      return false;
  } else if(_lexer.is_token(")") || _lexer.is_token("}")) {
    warning("Unexpected token %s at line %d", _lexer.token(), _lexer.line());
    return false;
  } else {
    const char* token(_lexer.token());
    if(_lexer.literal()) v = token; // Character string
    else if(strpbrk(token, "0123456789")) { // Has digits
      if(token[strspn(token, "-0123456789")]=='\0') v = atoi(token); // Integer
      else if(token[strspn(token, "-0123456789.")]=='\0') v = (float)atof(token); // Float
      else v = Symbol(token);
    } else if(_lexer.is_token("true")) v = true;
    else if(_lexer.is_token("false")) v = false;
    else v = Symbol(token);
    _lexer.next_token();
  }
  return true;
}
*/