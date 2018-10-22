#pragma once

#include "../container/Array.h"
#include "../container/Ref.h"
#include "../dynamic/Variable.h"
#include "ScriptGlobal.h"
#include "../stream/Stream.h"
#include "../text/Symbol.h"

namespace L {
  typedef void(*ScriptNativeFunction)(class ScriptContext&);

  enum ScriptOpCode : uint8_t {
    CopyLocal, // Copy local b to local a
    LoadConst, // Copy constant b to local a
    LoadGlobal, // Copy global with index constant b to local a
    StoreGlobal, // Set global with index constant a to local b
    LoadFun, // Creates new ScriptFunction in local a from current script and offset bc

    MakeObject, // Creates an associative array at local a
    GetItem, // Copy item from local a at index local b to local c
    SetItem, // Set item from local a at index local b to local c
    GetItemConst, // Copy item from local a at index const b to local c
    SetItemConst, // Set item from local a at index const b to local c

    MakeIterator, // Create an array iterator at local a from object at local b
    Iterate, // Put next key from iterator at local c to local a and next value to local b
    IterEndJump, //  Relative jump using bc as signed offset if iterator at local a is done

    Jump, // Relative jump using bc as signed offset
    CondJump, // Relative jump using bc as signed offset if local a is truthy
    CondNotJump, // Relative jump using bc as signed offset if local a is falsey

    Add, // Add local b to local a
    Sub, // Subtract local b to local a
    Mul, // Multiply local a by local b
    Div, // Divide local a by local b
    Mod, // Set local a to the remainder of local a divided by local b
    Inv, // Invert local a
    Not, // Invert local a logically

    LessThan, // Put in local a whether local b is lesser than local c
    LessEqual, // Put in local a whether local b is lesser than or equal to local c
    Equal, // Put in local a whether local b is equal to local c

    Call, // Call local a (with b parameters), function is at stack offset, return value will be at stack offset, self is at offset+1
    Return, // Return from current function

    // Optimization opcodes
    LoadBool, // Load boolean b to local a
    LoadInt, // Load integer bc (as float) to local a
    CondCopyLocal, // Copy local b to local a if local c is true
    CondLoadConst, // Copy constant b to local a if local c is true
    CondLoadGlobal, // Copy global b to local a if local c is true

  };
  struct ScriptInstruction {
    ScriptOpCode opcode;
    uint8_t a;
    union {
      struct { uint8_t b, c; };
      int16_t bc;
    };
  };
  struct Script {
    Array<Var> constants;
    Array<ScriptGlobal> globals;
    Array<ScriptInstruction> bytecode;

    uint8_t constant(const Var&);
    uint8_t global(Symbol);
  };
  struct ScriptFunction {
    typedef ScriptFunction Intermediate;
    Ref<Script> script;
    uintptr_t offset;
  };

  inline Stream& operator<=(Stream& s, const Script& v) { return s <= v.constants <= v.globals <= v.bytecode; }
  inline Stream& operator>=(Stream& s, Script& v) { return s >= v.constants >= v.globals >= v.bytecode; }
  inline Stream& operator<=(Stream& s, const ScriptFunction& v) { return s <= v.script <= v.offset; }
  inline Stream& operator>=(Stream& s, ScriptFunction& v) { return s >= v.script >= v.offset; }
}
