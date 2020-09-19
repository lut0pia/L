#include <cstdlib>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/script/script.h>
#include <L/src/engine/Engine.h>

using namespace L;

static const Symbol lasm_symbol("lasm");

static inline bool is_horizontal_space(char c) {
  return c == ' ' || c == '\t';
}

static const char* ordinals[] {"", "first", "second", "third", "fourth"};

enum class LasmLexerState {
  Space, Literal, StartString, String,
};

#define IS_OPCODE(name) !strcmp(words[0], #name)

#define REQ_ARG_CNT(n) { \
  if(word_count != n + 1) {\
    warning("%s:%d: opcode '%s' requires %d arguments", slot.id, current_line_index, words[0], n); \
    return false; \
  } \
}

#define REQ_ARG_BYTE(n, name) uint8_t name; { \
  if(strspn(words[n], "0123456789") == strlen(words[n])) { \
    int tmp = atoi(words[n]); \
    if(tmp >= 0 && tmp < 256) { \
      name = uint8_t(tmp); \
    } else { \
      warning("%s:%d: opcode '%s' requires an integer between 0 and 255 as its %s argument", slot.id, current_line_index, words[0], ordinals[n]); \
      return false; \
    } \
  } else { \
    warning("%s:%d: opcode '%s' requires an integer as its %s argument", slot.id, current_line_index, words[0], ordinals[n]); \
    return false; \
  } \
}
#define REQ_ARG_CONST(n, name) uint16_t name = script.constant(get_value(words[n]));

#define REQ_ARG_OFF(n, absolute) \
  label_references.push(LabelReference {words[0], words[n], script.bytecode.size(), current_line_index, absolute})
#define REQ_ARG_RELOFF(n) REQ_ARG_OFF(n, false)
#define REQ_ARG_ABSOFF(n) REQ_ARG_OFF(n, true)

static Var get_value(const char* word) {
  const size_t len = strlen(word);
  if(strspn(word, "0123456789") == len) { // Number
    return (float)atof(word);
  } else if(*word == '"') { // String
    String str = word + 1;
    str.trim_right();
    return str;
  } else if(*word == '\'') { // Symbol
    return Symbol(word + 1);
  } else if(!strcmp(word, "true")) {
    return true;
  } else if(!strcmp(word, "false")) {
    return false;
  } else {
    return Var();
  }
}

static bool lasm_script_loader(ResourceSlot& slot, ScriptFunction& intermediate) {
  if(slot.ext != lasm_symbol) {
    return false;
  }

  Buffer buffer = slot.read_source_file();

  if(!buffer) {
    return false;
  }

  char* cur = (char*)buffer.data();
  const char* end = (const char*)buffer.data(buffer.size());

  intermediate.offset = 0;
  intermediate.script = ref<Script>();
  Script& script = *intermediate.script;

  struct LabelReference {
    const char* opcode;
    Symbol label;
    uintptr_t offset;
    uint32_t line;
    bool absolute;
  };

  Table<Symbol, uintptr_t> labels;
  Array<LabelReference> label_references;

  // Iterate over lines
  uint32_t line_index = 1;
  while(cur < end) {
    const char* words[8] {};
    uintptr_t word_count = 0;

    const uint32_t current_line_index = line_index;

    { // Parse words
      LasmLexerState state = LasmLexerState::Space;
      uintptr_t offset = 0;
      bool abort = false;
      while(!abort) {
        if(*cur == '\n' || *cur == '\r' || *cur == ';') {
          break;
        }
        switch(state) {
          case LasmLexerState::Space:
            if(!is_horizontal_space(*cur) || *cur == '"') {
              state = *cur == '"' ? LasmLexerState::StartString : LasmLexerState::Literal;
              words[word_count++] = cur;
              if(word_count == L_COUNT_OF(words)) {
                abort = true;
              }
            } else {
              *cur = '\0';
              cur++;
            }
            break;
          case LasmLexerState::Literal:
            if(is_horizontal_space(*cur)) {
              state = LasmLexerState::Space;
            } else {
              cur++;
            }
            break;
          case LasmLexerState::StartString:
            offset = 0;
            state = LasmLexerState::String;
            break;
          case LasmLexerState::String:
            *cur = cur[offset];
            if(*cur == '\\') {
              offset++;
              *cur = cur[offset];
              if(*cur == 'n') {
                *cur = '\n';
              }
            } else if(words[word_count - 1] != cur && *cur == '"') {
              if(offset > 0) {
                cur[1] = '\0';
                cur += offset;
              }
              state = LasmLexerState::Space;
            }
            cur++;
            break;
        }
      }
    }

    // Skip potential rest of line
    while(cur < end && (*cur != '\n' && *cur != '\r')) {
      *cur = '\0';
      cur++;
    }

    // Skip to next line
    while(cur < end && (*cur == '\n' || *cur == '\r')) {
      if(*cur == '\n') {
        line_index++;
      }
      *cur = '\0';
      cur++;
    }

    if(word_count == 1 && words[0][strlen(words[0]) - 1] == ':') {
      labels[Symbol(words[0], strlen(words[0]) - 1)] = script.bytecode.size();
    } else if(word_count > 0) {
      if(IS_OPCODE(copylocal)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {CopyLocal, dst, src});
      } else if(IS_OPCODE(loadconst)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_CONST(2, src);
        script.bytecode.push(ScriptInstruction {LoadConst, dst});
        script.bytecode.back().bcu16 = src;
      } else if(IS_OPCODE(loadglobal)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst);
        script.bytecode.push(ScriptInstruction {LoadGlobal, dst});
        script.bytecode.back().bcu16 = script.global(Symbol(words[2]));
      } else if(IS_OPCODE(storeglobal)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {StoreGlobal, src});
        script.bytecode.back().bcu16 = script.global(Symbol(words[1]));
      } else if(IS_OPCODE(loadfun)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_ABSOFF(2);
        script.bytecode.push(ScriptInstruction {LoadFun, dst});
      } else if(IS_OPCODE(loadouter)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {LoadOuter, dst, src});
      } else if(IS_OPCODE(storeouter)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {StoreOuter, dst, src});
      } else if(IS_OPCODE(captlocal)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {CaptLocal, dst, src});
      } else if(IS_OPCODE(captouter)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {CaptOuter, dst, src});
      } else if(IS_OPCODE(makeobject)) {
        REQ_ARG_CNT(1); REQ_ARG_BYTE(1, dst);
        script.bytecode.push(ScriptInstruction {MakeObject, dst});
      } else if(IS_OPCODE(getitem)) {
        REQ_ARG_CNT(3); REQ_ARG_BYTE(1, object); REQ_ARG_BYTE(2, index); REQ_ARG_BYTE(3, dst);
        script.bytecode.push(ScriptInstruction {GetItem, object, index, dst});
      } else if(IS_OPCODE(setitem)) {
        REQ_ARG_CNT(3); REQ_ARG_BYTE(1, object); REQ_ARG_BYTE(2, index); REQ_ARG_BYTE(3, src);
        script.bytecode.push(ScriptInstruction {SetItem, object, index, src});
      } else if(IS_OPCODE(makeiterator)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, object);
        script.bytecode.push(ScriptInstruction {MakeIterator, dst, object});
      } else if(IS_OPCODE(iterate)) {
        REQ_ARG_CNT(3); REQ_ARG_BYTE(1, key_dst); REQ_ARG_BYTE(2, val_dst); REQ_ARG_BYTE(3, iterator);
        script.bytecode.push(ScriptInstruction {Iterate, key_dst, val_dst, iterator});
      } else if(IS_OPCODE(iterendjump)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, iterator); REQ_ARG_RELOFF(2);
        script.bytecode.push(ScriptInstruction {IterEndJump, iterator});
      } else if(IS_OPCODE(jump)) {
        REQ_ARG_CNT(1); REQ_ARG_RELOFF(1);
        script.bytecode.push(ScriptInstruction {Jump});
      } else if(IS_OPCODE(condjump)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, cond); REQ_ARG_RELOFF(2);
        script.bytecode.push(ScriptInstruction {CondJump, cond});
      } else if(IS_OPCODE(condnotjump)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, cond); REQ_ARG_RELOFF(2);
        script.bytecode.push(ScriptInstruction {CondNotJump, cond});
      } else if(IS_OPCODE(add)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {Add, dst, src});
      } else if(IS_OPCODE(sub)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {Sub, dst, src});
      } else if(IS_OPCODE(mul)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {Mul, dst, src});
      } else if(IS_OPCODE(div)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, dst); REQ_ARG_BYTE(2, src);
        script.bytecode.push(ScriptInstruction {Div, dst, src});
      } else if(IS_OPCODE(inv)) {
        REQ_ARG_CNT(1); REQ_ARG_BYTE(1, local);
        script.bytecode.push(ScriptInstruction {Inv, local});
      } else if(IS_OPCODE(not)) {
        REQ_ARG_CNT(1); REQ_ARG_BYTE(1, local);
        script.bytecode.push(ScriptInstruction {Not, local});
      } else if(IS_OPCODE(lessthan)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, a); REQ_ARG_BYTE(2, b);
        script.bytecode.push(ScriptInstruction {LessThan, a, b});
      } else if(IS_OPCODE(lessequal)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, a); REQ_ARG_BYTE(2, b);
        script.bytecode.push(ScriptInstruction {LessEqual, a, b});
      } else if(IS_OPCODE(equal)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, a); REQ_ARG_BYTE(2, b);
        script.bytecode.push(ScriptInstruction {Equal, a, b});
      } else if(IS_OPCODE(call)) {
        REQ_ARG_CNT(2); REQ_ARG_BYTE(1, func); REQ_ARG_BYTE(2, parameter_count);
        script.bytecode.push(ScriptInstruction {Call, func, parameter_count});
      } else if(IS_OPCODE(return)) {
        REQ_ARG_CNT(0);
        script.bytecode.push(ScriptInstruction {Return});
      } else {
        warning("%s:%d: unknown opcode '%s'", slot.id, current_line_index, words[0]);
        return false;
      }
    }
  }

  // Resolve label references
  for(const LabelReference& label_reference : label_references) {
    if(uintptr_t* label_value = labels.find(label_reference.label)) {
      // In the case of relative offset we need to subtract one
      // to account for the instruction increment that always happens in the VM
      script.bytecode[label_reference.offset].bc16 = label_reference.absolute
        ? int16_t(*label_value)
        : int16_t(intptr_t(*label_value) - intptr_t(label_reference.offset) - 1);
    } else {
      warning("%s:%d: opcode '%s' requires a valid label", slot.id, label_reference.line, label_reference.opcode);
      return false;
    }
  }

  script.bytecode.push(ScriptInstruction {Return});

  return true;
}
void lasm_module_init() {
  ResourceLoading<ScriptFunction>::add_loader(lasm_script_loader);
}
