#include "LSCompiler.h"

#include <L/src/container/Ref.h>

using namespace L;

void LSCompiler::read(const char* text, size_t size, bool last_read) {
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
      if(_state == PostQuote) {
        L_ASSERT(v.is<Symbol>());
        const Symbol sym(v.as<Symbol>());
        v.make<Script::RawSymbol>().sym = sym;
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
void LSCompiler::reset() {
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
Script::CodeFunction LSCompiler::function() const {
  L_ASSERT(ready());
  Script::CodeFunction code_function {_code, 0};
  code_function.code.as<Array<Var>>().pop(); // Remove temporary void value
  Table<Symbol, uint32_t> local_table;
  apply_scope(code_function.code, local_table, code_function.local_count);
  return code_function;
}

void LSCompiler::apply_scope(Var& v, Table<Symbol, uint32_t>& local_table, uint32_t& local_index) {
  static const Symbol localSymbol("local"), funSymbol("fun"), foreachSymbol("foreach"), setSymbol("set");
  if(v.is<Array<Var>>()) {
    Array<Var>& array(v.as<Array<Var>>());
    if(array.size()>0 && array[0].is<Symbol>()) {
      const Symbol& sym(array[0].as<Symbol>());
      if(array.size()>1 && array[1].is<Symbol>() && sym==localSymbol) {
        array[0] = setSymbol;
        local_table[array[1].as<Symbol>()] = local_index++;
      } else if(sym==foreachSymbol) {
        if(array.size()>=4) // Value only
          local_table[array[1].as<Symbol>()] = local_index++;
        if(array.size()>=5) // Key value
          local_table[array[2].as<Symbol>()] = local_index++;
      } else if(sym==funSymbol) {
        if(array.size()>1) {
          Ref<Script::CodeFunction> fun;
          fun.make();
          Table<Symbol, uint32_t> fun_local_table;
          uint32_t fun_local_index(0);
          if(array.size()>2 && array[1].is<Array<Var> >()) // There's a list of parameter symbols
            for(const Var& sym : array[1].as<Array<Var> >()) {
              L_ASSERT(sym.is<Symbol>());
              fun_local_table[sym.as<Symbol>()] = fun_local_index++;
            }
          fun->code = array.back(); // The last part is always the code
          LSCompiler::apply_scope(fun->code, fun_local_table, fun_local_index);
          fun->local_count = fun_local_index;
          v = fun;
        }
        return;
      }
    }
    for(Var& e : array)
      apply_scope(e, local_table, local_index);
  } else if(v.is<Symbol>()) {
    if(uint32_t* found = local_table.find(v.as<Symbol>()))
      v = Script::Local {*found};
  }
}
