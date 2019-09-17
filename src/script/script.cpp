#include "script.h"

using namespace L;

uint16_t Script::constant(const Var& v) {
  for(uint16_t i(0); i < constants.size(); i++) {
    if(constants[i] == v) {
      return i;
    }
  }
  L_ASSERT(constants.size() < UINT16_MAX);
  constants.push(v);
  return uint8_t(constants.size() - 1);
}
uint16_t Script::global(Symbol sym) {
  for(uint8_t i(0); i < globals.size(); i++) {
    if(globals[i].name() == sym) {
      return i;
    }
  }
  L_ASSERT(globals.size() < UINT16_MAX);
  globals.push(sym);
  return uint8_t(globals.size() - 1);
}
