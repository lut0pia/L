#pragma once

#include "ScriptContext.h"

#define L_SCRIPT_METHOD(TYPE, NAME, NARGS, ...) \
  ScriptContext::type_value(Type<TYPE*>::description(), Symbol(NAME)) = \
  ScriptContext::type_value(Type<Handle<TYPE>>::description(), Symbol(NAME)) = \
  (ScriptNativeFunction)([](ScriptContext& c) { \
    if(c.param_count() < NARGS) { \
      warning("Too few arguments in method call '%s.%s'", #TYPE, NAME); \
    } \
    if(c.current_self().is<TYPE*>()) { \
      c.current_self().as<TYPE*>()->__VA_ARGS__; \
    } else if(c.current_self().is<Handle<TYPE>>()) { \
      c.current_self().as<Handle<TYPE>>()->__VA_ARGS__; \
    } else { \
      warning("Calling method '%s.%s' on wrong type '%s'", #TYPE, NAME, c.current_self().type()->name); \
    } \
  })
#define L_SCRIPT_RETURN_METHOD(TYPE, NAME, NARGS, ...) \
  ScriptContext::type_value(Type<TYPE*>::description(), Symbol(NAME)) = \
  ScriptContext::type_value(Type<Handle<TYPE>>::description(), Symbol(NAME)) = \
  (ScriptNativeFunction)([](ScriptContext& c) { \
    if(c.param_count() < NARGS) { \
      warning("Too few arguments in method call '%s.%s'", #TYPE, NAME); \
    } \
    if(c.current_self().is<TYPE*>()) { \
      c.return_value() = c.current_self().as<TYPE*>()->__VA_ARGS__; \
    } else if(c.current_self().is<Handle<TYPE>>()) { \
      c.return_value() = c.current_self().as<Handle<TYPE>>()->__VA_ARGS__; \
    } else { \
      warning("Calling method '%s.%s' on wrong type '%s'", #TYPE, NAME, c.current_self().type()->name); \
    } \
  })
