#include "Code_FunctionCall.h"

#include "VirtualMachine.h"

using namespace L;
using namespace Burp;

Code_FunctionCall::Code_FunctionCall(String code, Map<String,size_t>& vi, size_t& ni) {
  size_t tmp = code.find_first_of('(');
  function = &VirtualMachine::functions[code.substr(0,tmp)];
  List<String> part = code.substr(tmp+1,code.size()-tmp-2).escapedExplode(',',String::allbrackets);
  L_Iter(part,it)
  parameters.push(from(*it,vi,ni));
}
Variable Code_FunctionCall::execute(Array<Variable>& locals) {
  Array<Variable> paramVars(Code::multiExecute(parameters,locals));
  return (*function)->call(paramVars);
}
