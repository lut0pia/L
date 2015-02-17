#include "Code_ConstValue.h"

using namespace L;
using namespace Burp;

Code_ConstValue::Code_ConstValue(String code, Map<String,size_t>& vi, size_t& ni){
    value = fromConstExp(code);
}
Variable Code_ConstValue::execute(Vector<Variable>& BurpVariables){
    return value;
}
