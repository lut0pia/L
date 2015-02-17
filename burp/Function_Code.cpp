#include "Function_Code.h"

using namespace L;
using namespace Burp;

Function_Code::Function_Code(Ref<Code> code, size_t scopeSize)
    : code(code), scopeSize(scopeSize){}
Variable Function_Code::call(Vector<Variable>& parameters){
    parameters.resize(scopeSize);
    return code->execute(parameters);
}
