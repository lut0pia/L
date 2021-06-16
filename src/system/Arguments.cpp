#include "Arguments.h"

#include "../container/Table.h"
#include "../text/String.h"

using namespace L;

static Table<Symbol, Array<Symbol>> arguments;

void Arguments::init(int argc, const char* argv[]) {
  Symbol last_key;
  for(intptr_t i = 1; i < argc; i++) {
    const char* arg = argv[i];
    if(!strncmp(arg, "--", 2)) { // --key
      arg += 2;
    } else if(!strncmp(arg, "-", 1)) { // -key
      arg += 1;
    } else if(last_key) { // Value for last key
      arguments[last_key].push(arg);
      continue;
    } else {
      warning("Command line value '%s' has no key", arg);
      continue;
    }

    // Key or key=values
    if(const char* delim = strchr(arg, '=')) {
      Array<Symbol>& values = arguments[last_key = Symbol(arg, delim - arg)];
      delim += 1;
      while(const char* next_delim = strchr(delim, ',')) {
        values.push(Symbol(delim, next_delim - delim));
        delim = next_delim + 1;
      }
      values.push(delim);
    } else {
      arguments[last_key = arg];
    }
  }
}

bool Arguments::has(const Symbol& key) {
  return arguments.find(key) != nullptr;
}
Symbol Arguments::get(const Symbol& key) {
  if(Array<Symbol>* values = arguments.find(key)) {
    return (*values)[0];
  } else {
    return Symbol();
  }
}
void Arguments::get(const Symbol& key, Array<Symbol>& values) {
  if(Array<Symbol>* values_found = arguments.find(key)) {
    values = *values_found;
  }
}
