#include "Function_Pointer.h"

using namespace L;
using namespace Burp;

Function_Pointer::Function_Pointer(Variable (*f)(Array<Variable>&)){
    this->f = f;
}
Variable Function_Pointer::call(Array<Variable>& parameters){
    return f(parameters);
}
