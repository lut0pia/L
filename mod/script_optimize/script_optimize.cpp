#include <L/src/engine/Resource.inl>
#include <L/src/script/script.h>

using namespace L;

static bool is_jump_inst(ScriptInstruction inst) {
  return inst.opcode == IterEndJump || inst.opcode == Jump || inst.opcode == CondJump || inst.opcode == CondNotJump;
}
static void remove_instruction(Script& script, uintptr_t index) {
  for(uintptr_t i(0); i < script.bytecode.size(); i++) {
    ScriptInstruction& inst(script.bytecode[i]);
    // Shift every jump that should be affected
    if(is_jump_inst(inst)) {
      uintptr_t ji(i + intptr_t(inst.bc16));
      if(i < index && index <= ji) {
        inst.bc16--;
      } else if(ji <= index && index < i) {
        inst.bc16++;
      }
    }
    // Shift functions that should be affected
    else if(inst.opcode == LoadFun) {
      if(index < uintptr_t(inst.bc16)) {
        inst.bc16--;
      }
    }
  }
  script.bytecode.erase(index);
  if(index < script.bytecode_line.size()) {
    script.bytecode_line.erase(index);
  }
}
static void script_optimize(Script& script) {
  L_SCOPE_MARKER("script_optimize");
  { // Replace LoadConst by shortcuts when available
    for(ScriptInstruction& inst : script.bytecode) {
      if(inst.opcode == LoadConst) {
        const Var& const_value(script.constants[inst.bc8.b]);
        if(const_value.is<bool>()) {
          inst.opcode = LoadBool;
          inst.bc8.b = const_value.as<bool>();
        } else if(const_value.is<float>()) {
          const float float_value(const_value.as<float>());
          if(float_value == float(int16_t(float_value))) {
            inst.opcode = LoadInt;
            inst.bc16 = int16_t(float_value);
          }
        }
      }
    }
  }

  { // Remove null jumps
    for(uintptr_t i(0); i < script.bytecode.size(); i++) {
      if(is_jump_inst(script.bytecode[i]) && script.bytecode[i].bc16 == 0) {
        remove_instruction(script, i);
        i--;
      }
    }
  }

  { // Remove unused constants
    // Gather all used constants
    Table<uintptr_t, bool> used_constants;
    for(uintptr_t i(0); i < script.bytecode.size(); i++) {
      if(script.bytecode[i].opcode == LoadConst) {
        used_constants[script.bytecode[i].bcu16] = true;
      } else if(script.bytecode[i].opcode == GetItemConst || script.bytecode[i].opcode == SetItemConst) {
        used_constants[script.bytecode[i].bc8.b] = true;
      }
    }

    const size_t before_const_count(script.constants.size());
    uintptr_t after_i(0);
    for(uintptr_t before_i(0); before_i < before_const_count; before_i++) {
      if(!used_constants.find(before_i)) { // This constant is unused
        // Remove it from constant array
        script.constants.erase(after_i);

        // Walk through code to update constant indices
        for(uintptr_t j(0); j < script.bytecode.size(); j++) {
          ScriptInstruction& inst(script.bytecode[j]);
          if(inst.opcode == LoadConst && inst.bc8.b > after_i) {
            inst.bc8.b--;
          }
        }
      } else {
        after_i++;
      }
    }
  }
}
void script_optimize_transformer(const ResourceSlot&, ScriptFunction::Intermediate& intermediate) {
  script_optimize(*intermediate.script);
}

void script_optimize_module_init() {
  ResourceLoading<ScriptFunction>::add_transformer(script_optimize_transformer);
}
