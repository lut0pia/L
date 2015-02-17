#include "Scope.h"

using namespace L;
using namespace Dynamic;

List<Map<String,Var> > Scope::stack;

Scope::Scope() : scope((
    stack.push_front((stack.empty())?Map<String,Var>():stack.front()),
    stack.front())){}
Var& Scope::operator[](const String& k){
    return scope[k];
}
Scope::~Scope(){
    stack.pop_front();
}
