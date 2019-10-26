#pragma once

#include "ScriptContext.h"

#define L_SCRIPT_METHOD(type, name, n, ...) \
  ScriptContext::type_value(Type<type*>::description(), Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) { \
  L_ASSERT(c.param_count() >= n && c.current_self().is<type*>()); \
  c.current_self().as<type*>()->__VA_ARGS__; \
})
#define L_SCRIPT_RETURN_METHOD(type, name, n, ...) \
  ScriptContext::type_value(Type<type*>::description(), Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) { \
  L_ASSERT(c.param_count() >= n && c.current_self().is<type*>()); \
  c.return_value() = c.current_self().as<type*>()->__VA_ARGS__; \
})
