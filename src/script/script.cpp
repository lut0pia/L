#include "script.h"

using namespace L;

uint8_t Script::constant(const Var& v) {
  for(uint8_t i(0); i<constants.size(); i++) {
    if(constants[i]==v) {
      return i;
    }
  }
  L_ASSERT(constants.size()<255);
  constants.push(v);
  return uint8_t(constants.size()-1);
}
uint8_t Script::global(Symbol sym) {
  for(uint8_t i(0); i<globals.size(); i++) {
    if(globals[i].name()==sym) {
      return i;
    }
  }
  L_ASSERT(globals.size()<255);
  globals.push(sym);
  return uint8_t(globals.size()-1);
}
