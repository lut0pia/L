#include "Parser.h"

#include "../containers/StaticStack.h"

using namespace L;
using namespace Script;

Parser::Node::Node(uint symbol) : _symbol(symbol) {}
void Parser::Node::print() {
  static int indent(0);
  for(int i(0); i<indent; i++)
    out << '-';
  out << _symbol << '\n';
  indent++;
  for(int i(0); i<_children.size(); i++)
    _children[i]->print();
  indent--;
}

void Parser::startSymbol(const String& strSymbol) {
  _startSymbol = hash(strSymbol.c_str());
}
void Parser::addRule(const Array<const char*>& strRule) {
  Array<uint> rule;
  for(int i(0); i<strRule.size(); i++)
    rule.push(hash(strRule[i]));
  _rules.push(rule);
}
uint Parser::hash(const char* s) const {
  uint wtr(0);
  bool terminal(*s!='&');
  int c;
  while(c=*s++) wtr = ((wtr<<5)+wtr)+c;
  wtr <<= 2; // Let first bit free for marking
  if(terminal) wtr |= 1; // Mark hash
  return wtr;
}
uint Parser::index(uint hash) {
  if(hash&1) { // Terminal
    for(int i(0); i<_terminals.size(); i++)
      if(hash==_terminals[i])
        return i;
    _terminals.push(hash);
    return _terminals.size()-1;
  } else { // Non-terminal
    for(int i(0); i<_nonterminals.size(); i++)
      if(hash==_nonterminals[i])
        return i;
    _nonterminals.push(hash);
    return _nonterminals.size()-1;
  }
}
Ref<Parser::Node> Parser::parse(Lexer& lexer) {
  Ref<Parser::Node> wtr;
  StaticStack<512,uint> stack;
  stack.push(_startSymbol);
  do {
    uint symbol(hash(lexer.token()));
    if(stack.top()==symbol) { // Symbol matched
      stack.pop();
    } else { // Look for a rule
      uint rule(_table(index(stack.top()),index(symbol)));
    }
  } while(!stack.empty() && lexer.nextToken());
  if(!stack.empty()) throw Exception("Script: Reached end of stream unexpectedly");
  return wtr;
}
