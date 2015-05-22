#include "VirtualMachine.h"

#include <fstream>
#include "Code.h"
#include "../Exception.h"
#include "Function_Code.h"
#include "Function_Pointer.h"
#include "../stl.h"

using namespace L;
using namespace Burp;
using namespace std;
using Dynamic::Variable;

Map<String,Ref<Function> > VirtualMachine::functions;
Directory VirtualMachine::working;
bool VirtualMachine::exit;

void VirtualMachine::read(std::istream& stream) {
  enum {
    none,
    include,
    function,
    functionNamed,
    functionDeclaring
  } state = none;
  String buffer, functionName, functionParams;
  char c;
  size_t cblevel(0); // Curly bracket level
  exit = false;
  while(!exit && stream.good() && (c = stream.get())) {
    switch(state) {
      case none:
        if(c=='"') {
          state = include;
          buffer.clear();
        } else if(c=='(') {
          state = function;
          functionName = buffer;
          buffer.clear();
        } else if(isspace(c) || !stream.good()) {
          if(buffer=="check")
            safe();
          else if(buffer=="exit")
            exit = true;
          buffer.clear();
        } else if(c>0) // Escape the BOM
          buffer.push_back(c);
        break;
      case include:
        if(c=='"') {
          std::ifstream file((working.gPath()+buffer).c_str(),std::ios::binary);
          Directory tmp(working);
          working = (File(buffer).dir());
          read(file);
          working = tmp;
          state = none;
          buffer.clear();
        } else
          buffer.push_back(c);
        break;
      case function:
        if(c==')') {
          state = functionNamed;
          functionParams = buffer;
          buffer.clear();
        } else
          buffer.push_back(c);
        break;
      case functionNamed:
        if(c=='{') { // It's a declaration
          state = functionDeclaring;
          buffer = "{";
          cblevel = 1;
        } else { // It's a simple call to the function
          state = none;
          functions[functionName];
          if(safe()) { // There are no undeclared functions
            List<String> paramStrings(functionParams.explode(','));
            Array<Variable> parameters(paramStrings.size());
            for(size_t i(0); i<parameters.size(); i++) // The parameters are constant expressions that need to be interpreted
              parameters[i] = Code::fromConstExp(paramStrings[i].trim());
            functions[functionName]->call(parameters);
          } else throw Exception("Burp: Cannot call " + functionName);
        }
        break;
      case functionDeclaring:
        switch(c) {
          case '{':
            buffer.push_back(c);
            cblevel++;
            break;
          case '}':
            buffer.push_back(c);
            cblevel--;
            if(!cblevel) { // Reached the end of the body
              state = none;
              List<String> paramStrings(functionParams.explode(',')); // Get all parameters names
              Map<String,size_t> vi;
              size_t ni(0);
              L_Iter(paramStrings,it)
              Code::gIndex(it->trim(),vi,ni); // Create indexes for all of them
              Ref<Code> code(Code::from(buffer,vi,ni)); // Interpret the code
              functions[functionName] = new Function_Code(code,ni); // Create the function with next index as scope size
              buffer.clear();
            }
            break;
          case ' ':
          case '\n':
          case '\t':
          case '\r':
            if(buffer[buffer.size()-1]!=' ')
              buffer.push_back(' ');
            break;
          default:
            buffer.push_back(c);
            break;
        }
        break;
    }
  }
}
bool VirtualMachine::safe() {
  bool wtr(true);
  L_Iter(functions,it)
  if(it->second.null()) {
    wtr = false;
    throw Exception("Burp: Function " + it->first + " used but undefined.");
  }
  return wtr;
}

// Implementations of standard dynamic functions

Variable addition(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() + params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return Variable();
}
Variable subtraction(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() - params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational)
#undef TMP
  }
  return Variable();
}
Variable multiplication(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() * params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational)
#undef TMP
  }
  return Variable();
}
Variable division(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>() && params[1].as<T>()!=0) return params[0].as<T>() / params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational)
#undef TMP
  }
  return Variable();
}
Variable modulo(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() % params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational)
#undef TMP
  }
  return Variable();
}
Variable opposite(Array<Variable>& params) {
#define TMP(T) if(params[0].is<T>()) return -params[1].as<T>();
  TMP(bool) TMP(int) TMP(Rational)
#undef TMP
  return Variable();
}
Variable assignment(Array<Variable>& params) {
  if(params[0].is<Variable*>())
    return *params[0].as<Variable*>() = params[1];
  else if(params[0].is<Ref<Variable> >())
    return *params[0].as<Ref<Variable> >() = params[1];
  else return Variable();
}

Variable eq(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() == params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return false;
}
Variable ne(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() != params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return false;
}
Variable gt(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() > params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return false;
}
Variable lt(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() < params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return false;
}
Variable ge(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() >= params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return false;
}
Variable le(Array<Variable>& params) {
  if(params[0].type() == params[1].type()) {
#define TMP(T) if(params[0].is<T>()) return params[0].as<T>() <= params[1].as<T>();
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  }
  return false;
}

Variable indirection(Array<Variable>& params) {
  if(params[0].is<Variable*>())
    return *params[0].as<Variable*>();
  else if(params[0].is<Ref<Variable> >())
    return *params[0].as<Ref<Variable> >();
  else
    return Variable();
}
Variable output(Array<Variable>& params) {
  params.foreach([](const Variable& v) {
#define TMP(T) if(v.is<T>()){out << v.as<T>();return;}
    TMP(bool) TMP(int) TMP(Rational) TMP(String)
#undef TMP
  });
  return Variable();
}
Variable input(Array<Variable>& params) {
  return Code::fromConstExp(in.line());
}
Variable newAlloc(Array<Variable>& params) {
  return Ref<Variable>(new Variable(params[0]));
}
Variable dynCall(Array<Variable>& params) {
  if(params.size()>0 && params[0].is<String>()) {
    Array<Variable> dynParams(params);
    dynParams.erase(0);
    return VirtualMachine::functions[params[0].as<String>()]->call(dynParams);
  } else return Variable();
}
Variable typeOf(Array<Variable>& params) {
#define TMP(T,name) if(params[0].is<T>()) return name;
  TMP(bool,"bool")
  TMP(int,"integer")
  TMP(Rational,"rational")
  TMP(Ref<Variable>,"reference")
  TMP(String,"String")
  TMP(Variable*,"reference")
  TMP(Array<Variable>,"array")
#undef TMP
  return "unknown";
}
Variable rand(Array<Variable>& params) {
  return Rational(rand(),RAND_MAX);
}
/*
BurpVariable at(Array<BurpVariable> params){
    BurpVariable wtr;
    if(params.size()==2 && params[0].gType()==_pointer && params[0].g_pointer()->gType()==_array
       && params[1].gType()==_int && params[1].g_int()>=0
       && params[1].g_int()<params[0].g_pointer()->g_array().size())
        wtr = params[0].g_pointer()->g_array()[params[1].g_int()].reference();
    return wtr;
}
BurpVariable push_back(Array<BurpVariable> params){
    (params[0].g_pointer())->g_array().push_back(params[1]);
    return BurpVariable();
}
BurpVariable pop_back(Array<BurpVariable> params){
    (params[0].g_pointer())->g_array().pop_back();
    return BurpVariable();
}
BurpVariable size(Array<BurpVariable> params){
    BurpVariable wtr;
    if(params.size() && params[0].gType()==_pointer && params[0].g_pointer()->gType()==_array)
        wtr = (int)params[0].g_pointer()->g_array().size();
    return wtr;
}
*/
void VirtualMachine::init() {
  Dynamic::Cast::init();
  Dynamic::Cast::declare<int,Rational>();
  Dynamic::Cast::declare<String,Rational>();
  functions["..+"] =               new Function_Pointer(addition);
  functions["..-"] =               new Function_Pointer(subtraction);
  functions["..*"] =               new Function_Pointer(multiplication);
  functions["../"] =               new Function_Pointer(division);
  functions["..%"] =               new Function_Pointer(modulo);
  functions["..="] =               new Function_Pointer(assignment);
  functions["..=="] =              new Function_Pointer(eq);
  functions["..!="] =              new Function_Pointer(ne);
  functions["..>"] =               new Function_Pointer(gt);
  functions["..<"] =               new Function_Pointer(lt);
  functions["..>="] =              new Function_Pointer(ge);
  functions["..<="] =              new Function_Pointer(le);
  functions[".-"] =                new Function_Pointer(opposite);
  functions[".*"] =                new Function_Pointer(indirection);
  functions[".<"] =                new Function_Pointer(output);
  functions["<"] =                 new Function_Pointer(output);
  functions["in"] =                new Function_Pointer(input);
  functions[".new"] =              new Function_Pointer(newAlloc);
  functions["call"] =              new Function_Pointer(dynCall);
  functions["typeof"] =            new Function_Pointer(typeOf);
  functions["rand"] =              new Function_Pointer(rand);
  /*

  functions["..->"] =              new Function_Pointer(at);
  functions["push_back"] =         new Function_Pointer(push_back);
  functions["pop_back"] =          new Function_Pointer(pop_back);
  functions["size"] =              new Function_Pointer(size);
  */
}
