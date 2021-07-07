#pragma once

#include "../container/Array.h"
#include "../container/Ref.h"
#include "../dynamic/Variable.h"
#include "ScriptGlobal.h"
#include "../stream/Stream.h"
#include "../text/String.h"
#include "../text/Symbol.h"

namespace L {
  typedef void(*ScriptNativeFunction)(class ScriptContext&);
  typedef bool(*ScriptGetItemFunction)(const ScriptContext&, const Var& object, const Var& index, Var& value);
  typedef bool(*ScriptSetItemFunction)(const ScriptContext&, Var& object, const Var& index, const Var& value);

  enum ScriptOpCode : uint8_t {
    CopyLocal, // Copy local b to local a
    LoadConst, // Copy constant bcu16 to local a
    LoadGlobal, // Copy global with index constant bcu16 to local a
    StoreGlobal, // Set global with index constant bcu16 to local a
    LoadFun, // Creates new ScriptFunction in local a from current script and offset bc

    LoadOuter, // Copy outer b to local a
    StoreOuter, // Copy local b to outer a
    CaptLocal, // Captures local b as next outer in function at local a
    CaptOuter, // Captures outer b as next outer in function at local a

    MakeObject, // Creates an associative array at local a
    MakeArray, // Creates an array at local a
    GetItem, // Copy item from local a at index local b to local c
    SetItem, // Set item from local a at index local b to local c
    PushItem, // Push item from local b to array at local a

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
    CondCopyLocal, // Copy local b to local a if local c is true
    CondLoadConst, // Copy constant b to local a if local c is true
    CondLoadGlobal, // Copy global b to local a if local c is true
    LoadBool, // Load boolean b to local a
    LoadInt, // Load integer bc (as float) to local a
    GetItemConst, // Copy item from local a at index const b to local c
    SetItemConst, // Set item from local a at index const b to local c
  };
  struct ScriptInstruction {
    ScriptOpCode opcode;
    uint8_t a;
    union {
      struct {
        uint8_t b, c;
      } bc8;
      int16_t bc16;
      uint16_t bcu16;
    };
  };
  struct Script {
    Array<Var> constants;
    Array<ScriptGlobal> globals;
    Array<ScriptInstruction> bytecode;

#if !L_RLS
    String source_id;
    Array<uint32_t> bytecode_line;
    Array<String> source_lines;
#endif

    uint16_t constant(const Var&);
    uint16_t global(Symbol);
    void print(Stream&);
  };
  struct ScriptOuter {
    uintptr_t offset;
    Var value;
  };
  struct ScriptFunction {
    typedef ScriptFunction Intermediate;
    Ref<Script> script;
    uintptr_t offset;
    Array<Ref<ScriptOuter>> outers;
  };

  inline void script_write(Stream& s, const Script& v) { s <= v.constants <= v.globals <= v.bytecode; }
  inline void script_read(Stream& s, Script& v) { s >= v.constants >= v.globals >= v.bytecode; }
  inline void resource_write(Stream& s, const ScriptFunction& v) {
    script_write(s, *v.script);
  }
  inline void resource_read(Stream& s, ScriptFunction& v) {
    if(!v.script.is_valid()) {
      v.script = ref<Script>();
    }
    script_read(s, *v.script);
    v.offset = 0;
  }

#if !L_RLS
  inline void script_write_dev(Stream& s, const Script& v) { s <= v.source_id <= v.bytecode_line <= v.source_lines; }
  inline void script_read_dev(Stream& s, Script& v) { s >= v.source_id >= v.bytecode_line >= v.source_lines; }
  inline void resource_write_dev(Stream& s, const ScriptFunction& v) {
    script_write_dev(s, *v.script);
  }
  inline void resource_read_dev(Stream& s, ScriptFunction& v) {
    if(!v.script.is_valid()) {
      v.script = ref<Script>();
    }
    script_read_dev(s, *v.script);
    v.offset = 0;
  }
#endif

  void init_script_standard_functions();
}
