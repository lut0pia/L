#include "Context.h"

using namespace L;
using namespace Script;

void Context::read(Stream& stream) {
  Script::Lexer lexer(stream);
  Var v;
  lexer.nextToken();
  while(!stream.end()) {
    read(v,lexer);
    execute(v,NULL);
  }
}
void Context::read(Var& v, Lexer& lexer) {
  if(lexer.acceptToken("(")) { // It's a list of expressions
    v = Array<Var>();
    int i(0);
    while(!lexer.acceptToken(")"))
      read(v[i++],lexer);
  } else {
    String token(lexer.token());
    if(function(token))
      v = *function(token);
    else v = token;
    lexer.nextToken();
  }
}
void Context::declare(const String& name, const Var& code) {
  *function(name) = code;
}
Var* Context::function(const String& name) {
  if(_functions[name].null())
    _functions[name] = new Var();
  return _functions[name];
}
Var Context::execute(const Var& code, Var* stack) {
  //out << code << '\n';
  if(code.is<Array<Var> >()) { // Function call
    const Var& handle(execute(code.as<Array<Var> >()[0],stack));
    if(handle.is<Native>())
      handle.as<Native>()(stack);
  }
  return code;
}
