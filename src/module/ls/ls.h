#pragma once

#include <L/src/container/Array.h>
#include <L/src/dynamic/Variable.h>
#include <L/src/text/Symbol.h>

struct RawSymbol {
  L::Symbol sym;
};
struct AccessChain {
  L::Array<L::Var> array;
  char last_access_type; // .:[
};