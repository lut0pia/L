#include "Context.h"

#include "code/Base.h"
#include "../streams/FileStream.h"

using namespace L;
using namespace Script;

Context::Context() {
  _parser.startSymbol("&block");
  _parser.addRule(Array<const char*>::make("&block","{","&inblock","}"));
  _parser.addRule(Array<const char*>::make("&inblock","&instruction","&inblock"));
  _parser.addRule(Array<const char*>::make("&inblock"));
  _parser.addRule(Array<const char*>::make("&instruction","&statement",";"));
  _parser.addRule(Array<const char*>::make("&statement","&assignment"));
  _parser.addRule(Array<const char*>::make("&statement","&expression"));
  _parser.addRule(Array<const char*>::make("&assignment","&ident","=","&statement"));
  _parser.addRule(Array<const char*>::make("&expression","&value"));
  _parser.addRule(Array<const char*>::make("&expression","&expression","+","&expression"));
  _parser.addRule(Array<const char*>::make("&value","&ident"));
  _parser.addRule(Array<const char*>::make("&value","&const"));
}
void Context::read(const File& file) {
  FileStream stream(file,"rb");
  Lexer lexer(stream);
  while(lexer.nextToken()) {
    out << lexer.token() << '\n';
    if(lexer.literal()) { // Include
      read(File(lexer.token()));
    } else if(lexer.isToken("{")) { // Execute
      _parser.parse(lexer);
    } else { // Declare
      String name(lexer.token()); // Fetch function name
      lexer.nextToken();
      lexer.expectToken("(");
      while(!lexer.acceptToken(")"))
        lexer.nextToken();
      _parser.parse(lexer)->print();
    }
  }
}

void Context::declare(const String& name, Ref<Code::Base> code) {
  *function(name) = code;
}
Ref<Code::Base>* Context::function(const String& name) {
  if(_functions[name].null())
    _functions[name] = new Ref<Code::Base>();
  return _functions[name];
}
