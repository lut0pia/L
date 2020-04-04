#pragma once

#include "ScriptContext.h"

#define L_SCRIPT_METHOD_INTERNAL(PREFIX, TYPE, NAME, NARGS, ...) \
  ScriptContext::type_value(Type<Handle<TYPE>>::description(), Symbol(NAME)) = \
  (ScriptNativeFunction)([](ScriptContext& c) { \
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
