#pragma once

#include "ScriptContext.h"

#define L_SCRIPT_FUNCTION(NAME, NARGS, ...) \
  register_script_function(NAME, \
  [](ScriptContext& c) { \
    if(c.param_count() < NARGS) { \
      c.warning("Too few arguments in function call '%s'", NAME); \
    } \
    __VA_ARGS__; \
  })

#define L_SCRIPT_METHOD_INTERNAL(PREFIX, TYPE, NAME, NARGS, ...) \
  register_script_method<Handle<TYPE>>(NAME, \
  [](ScriptContext& c) { \
    if(c.param_count() < NARGS) { \
      c.warning("Too few arguments in method call '%s.%s'", #TYPE, NAME); \
    } \
    if(Handle<TYPE>* handle = c.current_self().try_as<Handle<TYPE>>()) { \
      if(handle->is_valid()) { \
        PREFIX (*handle)->__VA_ARGS__; \
      } else { \
        c.warning("Calling method '%s.%s' on invalid handle", #TYPE, NAME); \
      } \
    } else { \
      c.warning("Calling method '%s.%s' on wrong type '%s'", #TYPE, NAME, c.current_self().type()->name); \
    } \
  })
#define L_SCRIPT_METHOD(TYPE, NAME, NARGS, ...) L_SCRIPT_METHOD_INTERNAL(, TYPE, NAME, NARGS, __VA_ARGS__)
#define L_SCRIPT_RETURN_METHOD(TYPE, NAME, NARGS, ...) L_SCRIPT_METHOD_INTERNAL(c.return_value() =, TYPE, NAME, NARGS, __VA_ARGS__)

namespace L {
  template <class T>
  inline void register_script_method(const char* name, ScriptNativeFunction func) {
    ScriptContext::type_value(Type<T>::description(), Symbol(name)) = func;
  }
  inline void register_script_function(const char* name, ScriptNativeFunction func) {
    ScriptGlobal(Symbol(name)) = func;
  }
}