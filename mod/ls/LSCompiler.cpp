#include "LSCompiler.h"

#include <L/src/stream/CFileStream.h>

using namespace L;

static const Symbol local_symbol("local"), set_symbol("set"), object_symbol("{object}"), array_symbol("[array]"),
fun_symbol("fun"), return_symbol("return"), foreach_symbol("foreach"), do_symbol("do"), if_symbol("if"), self_symbol("self"),
switch_symbol("switch"), while_symbol("while"), and_symbol("and"), or_symbol("or"), not_symbol("not"),
add_symbol("+"), sub_symbol("-"), mul_symbol("*"), div_symbol("/"), mod_symbol("%"),
add_assign_symbol("+="), sub_assign_symbol("-="), mul_assign_symbol("*="), div_assign_symbol("/="), mod_assign_symbol("%="),
less_symbol("<"), less_equal_symbol("<="), equal_symbol("="), greater_symbol(">"), greater_equal_symbol(">="), not_equal_symbol("<>");

bool LSCompiler::read(const char* text, size_t size) {
  _source_lines.clear();
  _source_lines.push();
  for(uintptr_t i = 0; i < size; i++) {
    const char c = text[i];
    if(c == '\n') {
      _source_lines.push();
    } else if(c != '\r' && !(_source_lines.back().size() == 0 && Stream::isspace(c))) {
      _source_lines.back().push(c);
    }
  }
  return _parser.read(_context, text, size);
}

bool LSCompiler::compile(ScriptFunction& script_function) {
  { // Clean previous state
    for(Function* function : _functions) {
      Memory::delete_type(function);
    }
    _functions.clear();
    _script = ref<Script>();
#if !L_RLS
    _script->source_id = _context;
    _script->source_lines = _source_lines;
#endif
  }

  { // Compiling
    Function& main_function(make_function(_parser.finish()));
    resolve_locals(main_function, main_function.code);
    if(!compile_function(main_function)) {
      return false;
    }
  }

  { // Bundling functions together
    for(Function* function : _functions) {
      function->push(function->code, ScriptInstruction {Return}); // Better safe than sorry
      function->bytecode_offset = uint32_t(_script->bytecode.size());
      _script->bytecode.insertArray(_script->bytecode.size(), function->bytecode);
#if !L_RLS
      _script->bytecode_line.insertArray(_script->bytecode_line.size(), function->bytecode_line);
#endif
    }
  }

  { // Linking
    for(ScriptInstruction& inst : _script->bytecode) {
      if(inst.opcode == LoadFun) {
        inst.bc16 = int16_t(_functions[inst.bc16]->bytecode_offset);
      }
    }
  }

  // Use for debugging
  //_script->print(out);

  script_function.script = _script;
  script_function.offset = 0;
  return true;
}
LSCompiler::Function& LSCompiler::make_function(const LSParser::Node& code, Function* parent) {
  _functions.push();
  Function*& function(_functions.back());
  function = Memory::new_type<Function>();
  function->parent = parent;
  function->code = code;
  function->local_count = 2; // Account for return value and self
  function->local_table[self_symbol] = 1; // Self is always at offset 1
  return *function;
}
bool LSCompiler::find_outer(Function& func, const Symbol& symbol, uint8_t& outer) {
  if(func.parent) {
    if(const Symbol* outer_ptr = func.outers.find(symbol)) {
      outer = uint8_t(outer_ptr - func.outers.begin());
      return true;
    } else if(func.parent->local_table.find(symbol)) {
      outer = uint8_t(func.outers.size());
      func.outers.push(symbol);
      return true;
    } else if(find_outer(*func.parent, symbol, outer)) {
      outer = uint8_t(func.outers.size());
      func.outers.push(symbol);
      return true;
    }
  }
  return false;
}
void LSCompiler::resolve_locals(Function& func, const LSParser::Node& code) {
  if(code.children.size() > 0 && code.children[0].value.is<Symbol>()) { // May be a special construct or a function call
    Symbol sym(code.children[0].value.as<Symbol>());

    if(sym == local_symbol) { // Local var declaration
      L_ASSERT(code.children[1].value.is<Symbol>());
      func.local_table[code.children[1].value.as<Symbol>()] = func.local_count++;
    } else if(sym == foreach_symbol) {
      if(code.children.size() >= 4) // Value only
        func.local_table[code.children[1].value.as<Symbol>()] = func.local_count++;
      if(code.children.size() >= 5) // Key value
        func.local_table[code.children[2].value.as<Symbol>()] = func.local_count++;
    } else if(sym == fun_symbol) {
      return;
    }
  }
  for(const LSParser::Node& node : code.children) {
    resolve_locals(func, node);
  }
}
bool LSCompiler::compile(Function& func, const LSParser::Node& node, uint8_t offset) {
  if(node.children.size() > 0) {
    const Array<LSParser::Node>& children = node.children;
    L_ASSERT(children.size() > 0);
    if(node.type == LSParser::NodeType::Access) {
      compile_access_chain(func, node.children, offset, true);
    } else if(children[0].value.is<Symbol>()) { // May be a special construct or a function call
      const Symbol sym = children[0].value.as<Symbol>();

      if(sym == set_symbol || sym == local_symbol) {
        if(children.size() >= 3) {
          compile(func, children[2], offset);
          compile_assignment(func, children[1], offset, offset + 1);
        }
      } else if(sym == object_symbol) {
        func.push(node, ScriptInstruction {MakeObject, offset});
        for(uintptr_t i = 1; i < children.size(); i += 2) {
          compile(func, children[i], offset + 1); // Key
          compile(func, children[i + 1], offset + 2); // Value
          func.push(node, ScriptInstruction {SetItem, offset, uint8_t(offset + 1), uint8_t(offset + 2)});
        }
      } else if(sym == array_symbol) {
        func.push(node, ScriptInstruction {MakeArray, offset});
        for(uintptr_t i = 1; i < children.size(); i++) {
          compile(func, children[i], offset + 1); // Value
          func.push(node, ScriptInstruction {PushItem, offset, uint8_t(offset + 1)});
        }
      } else if(sym == if_symbol) { // If
        for(uintptr_t i(1); i < children.size() - 1; i += 2) {
          compile(func, children[i], offset); // Put condition result at offset

          // If the result is true, jump right afterwards
          func.push(node, ScriptInstruction {CondJump, offset});
          func.bytecode.back().bc16 = 1;

          // If the result is false, jump after the execution block (will get updated after compilation)
          func.push(node, ScriptInstruction {Jump});
          const uintptr_t if_not_jump(func.bytecode.size() - 1);

          compile(func, children[i + 1], offset);

          // Handle else branch
          const bool else_branch(!(children.size() & 1) && i == children.size() - 3);
          if(else_branch) {
            // If the result is false, jump after the execution block (will get updated after compilation)
            func.push(node, ScriptInstruction {Jump});
            const uintptr_t else_avoid_jump(func.bytecode.size() - 1);

            // This is where we jump if the condition hasn't been met before
            func.bytecode[if_not_jump].bc16 = int16_t(func.bytecode.size() - if_not_jump) - 1;

            compile(func, children.back(), offset);

            // This is where we jump to avoid the else branch (condition was met before)
            func.bytecode[else_avoid_jump].bc16 = int16_t(func.bytecode.size() - else_avoid_jump) - 1;
          } else { // No else branch
            // This is where we jump if the condition hasn't been met before
            func.bytecode[if_not_jump].bc16 = int16_t(func.bytecode.size() - if_not_jump) - 1;
          }
        }
      } else if(sym == while_symbol) { // While
        const uintptr_t start_index(func.bytecode.size());
        // Compute condition at offset
        compile(func, children[1], offset);

        // If the result is true, jump right afterwards
        func.push(node, ScriptInstruction {CondJump, offset});
        func.bytecode.back().bc16 = 1;

        // If the result is false, jump after the execution block (will get updated after compilation)
        func.push(node, ScriptInstruction {Jump});
        const uintptr_t if_not_jump(func.bytecode.size() - 1);

        compile(func, children[2], offset);

        // Jump back to the start
        func.push(node, ScriptInstruction {Jump});
        func.bytecode.back().bc16 = int16_t(start_index) - int16_t(func.bytecode.size());

        // This is where we jump if the condition hasn't been met before
        func.bytecode[if_not_jump].bc16 = int16_t(func.bytecode.size() - if_not_jump) - 1;
      } else if(sym == foreach_symbol) {
        if(children.size() < 4) {
          warning("ls: %s:%d: foreach is missing parameters: (foreach [key] value iterable statement)", _context.begin(), 0);
          return false;
        }

        const LSParser::Node& key = children.size() == 4 ? children[1] : children[1];
        const LSParser::Node& value = children.size() == 4 ? children[1] : children[2];
        const LSParser::Node& iterable = children.size() == 4 ? children[2] : children[3];
        const LSParser::Node& statement = children.size() == 4 ? children[3] : children[4];

        // Compute object at offset
        compile(func, iterable, offset);
        func.push(node, ScriptInstruction {MakeIterator, uint8_t(offset + 1), offset});

        // If the iteration has ended, jump after the loop
        func.push(node, ScriptInstruction {IterEndJump, uint8_t(offset + 1)});
        const uintptr_t end_jump(func.bytecode.size() - 1);

        // Get next key/value pair
        const uint8_t key_local(*func.local_table.find(key.value.as<Symbol>()));
        const uint8_t value_local(*func.local_table.find(value.value.as<Symbol>()));
        func.push(node, ScriptInstruction {Iterate, key_local, value_local, uint8_t(offset + 1)});

        // Execute loop body
        compile(func, statement, offset + 2);

        // Jump back to beginning of loop
        func.push(node, ScriptInstruction {Jump});
        func.bytecode.back().bc16 = int16_t(end_jump) - int16_t(func.bytecode.size());

        // This is where we jump if the iterator has reached the end
        func.bytecode[end_jump].bc16 = int16_t(func.bytecode.size() - end_jump) - 1;
      } else if(sym == switch_symbol) { // Switch
        // Put first compare value at offset
        compile(func, children[1], offset);
        Array<uintptr_t> cond_jumps; // Will jump to appropriate code block if condition was met
        Array<uintptr_t> end_jumps; // Will jump *after* the switch

        // Start by doing comparisons until one matches
        for(uintptr_t i(2); i < children.size() - 1; i += 2) {
          // Put second compare value at offset+1
          compile(func, children[i], offset + 1);
          // Replace second compare value by comparison result
          func.push(node, ScriptInstruction {Equal, uint8_t(offset + 1), offset, uint8_t(offset + 1)});
          // Use result to jump conditionally
          func.push(node, ScriptInstruction {CondJump, uint8_t(offset + 1)});
          cond_jumps.push(func.bytecode.size() - 1);
        }

        // Compile the default case if there's one
        // We'll fallthrough here if none of the conditions were met
        if((children.size() & 1) != 0) {
          // Default case is always at the end of the switch
          compile(func, children.back(), offset);
        }

        // Jump to after the switch
        func.push(node, ScriptInstruction {Jump});
        end_jumps.push(func.bytecode.size() - 1);

        // Compile other cases
        for(uintptr_t i(2); i < children.size() - 1; i += 2) {
          // Update corresponding conditional jump to jump here
          const uintptr_t cond_jump(cond_jumps[(i - 2) / 2]);
          func.bytecode[cond_jump].bc16 = int16_t(func.bytecode.size() - cond_jump) - 1;

          // Compile the code
          compile(func, children[i + 1], offset);

          // Jump to after the switch
          func.push(node, ScriptInstruction {Jump});
          end_jumps.push(func.bytecode.size() - 1);
        }

        // Update all end jumps to jump here
        for(uintptr_t end_jump : end_jumps) {
          func.bytecode[end_jump].bc16 = int16_t(func.bytecode.size() - end_jump) - 1;
        }
      } else if(sym == and_symbol) { // And
        Array<uintptr_t> end_jumps; // Will jump *after* the and
        for(uintptr_t i(1); i < children.size(); i++) {
          compile(func, children[i], offset);
          func.push(node, ScriptInstruction {CondNotJump, offset});
          end_jumps.push(func.bytecode.size() - 1);
        }

        // Update all end jumps to jump here
        for(uintptr_t end_jump : end_jumps) {
          func.bytecode[end_jump].bc16 = int16_t(func.bytecode.size() - end_jump) - 1;
        }
      } else if(sym == or_symbol) { // Or
        Array<uintptr_t> end_jumps; // Will jump *after* the or
        for(uintptr_t i = 1; i < children.size(); i++) {
          compile(func, children[i], offset);
          func.push(node, ScriptInstruction {CondJump, offset});
          end_jumps.push(func.bytecode.size() - 1);
        }

        // Update all end jumps to jump here
        for(uintptr_t end_jump : end_jumps) {
          func.bytecode[end_jump].bc16 = int16_t(func.bytecode.size() - end_jump) - 1;
        }
      } else if(sym == not_symbol) { // Not
        compile(func, children[1], offset);
        func.push(node, ScriptInstruction {Not, offset});
      } else if(sym == fun_symbol) { // Function
        if(children.size() > 1) {
          // Make a new function from the last item of the array
          // because the last part is always the code
          Function& new_function(make_function(children.back(), &func));

          { // Deal with potential parameters
            for(uintptr_t i = 1; i < children.size() - 1; i++) {
              if(children[i].value.is<Symbol>()) {
                new_function.local_table[children[i].value.as<Symbol>()] = new_function.local_count++;
              } else {
                warning("ls: %s:%d: Function parameter names must be symbols", _context.begin(), children[i].line);
                return false;
              }
            }
          }

          // Save index of function in LoadFun to allow linking later
          func.push(node, ScriptInstruction {LoadFun, offset});
          func.bytecode.back().bc16 = int16_t(_functions.size() - 1);

          resolve_locals(new_function, new_function.code);
          compile_function(new_function);

          // Capture outers
          for(const Symbol& symbol : new_function.outers) {
            if(const uint8_t* local = func.local_table.find(symbol)) {
              func.push(node, ScriptInstruction {CaptLocal, offset, *local});
            } else if(const Symbol* outer_ptr = func.outers.find(symbol)) {
              func.push(node, ScriptInstruction {CaptOuter, offset, uint8_t(outer_ptr - func.outers.begin())});
            }
          }
        }
        return true;
      } else if(sym == return_symbol) {
        for(uintptr_t i = 1; i < children.size(); i++) {
          compile(func, children[i], uint8_t(offset + i - 1));
        }
        for(uintptr_t i = 1; i < children.size(); i++) {
          func.push(node, ScriptInstruction {CopyLocal, uint8_t(i - 1), uint8_t(offset + i - 1)});
        }
        func.push(node, ScriptInstruction {Return});
      } else if(sym == add_assign_symbol) {
        compile_op_assign(func, children, offset, Add);
      } else if(sym == sub_assign_symbol) {
        compile_op_assign(func, children, offset, Sub);
      } else if(sym == mul_assign_symbol) {
        compile_op_assign(func, children, offset, Mul);
      } else if(sym == div_assign_symbol) {
        compile_op_assign(func, children, offset, Div);
      } else if(sym == mod_assign_symbol) {
        compile_op_assign(func, children, offset, Mod);
      } else if(sym == add_symbol) { // Addition
        compile_operator(func, children, offset, Add);
      } else if(sym == sub_symbol) { // Subtraction and invert
        if(children.size() > 2) {
          compile_operator(func, children, offset, Sub);
        } else {
          compile(func, children[1], offset);
          func.push(node, ScriptInstruction {Inv, offset});
        }
      } else if(sym == mul_symbol) { // Multiplication
        compile_operator(func, children, offset, Mul);
      } else if(sym == div_symbol) { // Division
        compile_operator(func, children, offset, Div);
      } else if(sym == mod_symbol) { // Modulo
        compile_operator(func, children, offset, Mod);
      } else if(sym == less_symbol) { // Less
        compile_comparison(func, children, offset, LessThan);
      } else if(sym == less_equal_symbol) { // Less equal
        compile_comparison(func, children, offset, LessEqual);
      } else if(sym == equal_symbol) { // Equal
        compile_comparison(func, children, offset, Equal);
      } else if(sym == greater_symbol) { // Greater
        compile_comparison(func, children, offset, LessEqual, true);
      } else if(sym == greater_equal_symbol) { // Greater equal
        compile_comparison(func, children, offset, LessThan, true);
      } else if(sym == not_equal_symbol) { // Not equal
        compile_comparison(func, children, offset, Equal, true);
      } else if(sym == do_symbol) {
        for(uint32_t i(1); i < children.size(); i++) {
          compile(func, children[i], offset);
        }
      } else { // It's a function call
        compile_function_call(func, children, offset);
      }
    } else { // It's also a function call
      compile_function_call(func, children, offset);
    }
  } else if(node.type == LSParser::NodeType::Value && node.value.is<Symbol>()) {
    uint8_t outer;
    if(const uint8_t* local = func.local_table.find(node.value.as<Symbol>())) {
      func.push(node, ScriptInstruction {CopyLocal, offset, *local});
    } else if(find_outer(func, node.value.as<Symbol>(), outer)) {
      func.push(node, ScriptInstruction {LoadOuter, offset, outer});
    } else {
      func.push(node, ScriptInstruction {LoadGlobal, offset});
      func.bytecode.back().bcu16 = _script->global(node.value.as<Symbol>());
    }
  } else {
    func.push(node, ScriptInstruction {LoadConst, offset});
    func.bytecode.back().bcu16 = _script->constant(node.value);
  }

  return true;
}
bool LSCompiler::compile_function(Function& func) {
  // Use local_count as start offset to avoid overwriting parameters
  const uint8_t func_offset(func.local_count);

  if(!compile(func, func.code, func_offset)) {
    return false;
  }

  // We're compiling to target func_offset as a return value,
  // copy that to the actual return value (0)
  func.push(func.code, ScriptInstruction {CopyLocal, 0, func_offset});

  return true;
}
void LSCompiler::compile_access_chain(Function& func, const Array<LSParser::Node>& array, uint8_t offset, bool get) {
  L_ASSERT(array.size() >= 2);
  compile(func, array[0], offset + 1); // Object
  compile(func, array[1], offset + 2); // Index
  for(uint32_t i(2); i < array.size(); i++) {
    // Put Object[Index] where Object was
    func.push(array[i], ScriptInstruction {GetItem, uint8_t(offset + 1), uint8_t(offset + 2), uint8_t(offset + 1)});
    // Replace Index with next index in array
    compile(func, array[i], offset + 2);
  }
  if(get) {
    // Do final access
    func.push(array.back(), ScriptInstruction {GetItem, uint8_t(offset + 1), uint8_t(offset + 2), offset});
  } else {
    func.push(array.back(), ScriptInstruction {CopyLocal, offset, uint8_t(offset + 2)});
  }
}
void LSCompiler::compile_function_call(Function& func, const Array<LSParser::Node>& array, uint8_t offset) {
  compile(func, array[0], offset);
  // Self should already be at offset+1 if specified
  for(uint32_t i(1); i < array.size(); i++) {
    compile(func, array[i], uint8_t(offset + i + 1));
  }
  func.push(array[0], ScriptInstruction {Call, offset, uint8_t(array.size() - 1)});
}
void LSCompiler::compile_assignment(Function& func, const LSParser::Node& dst, uint8_t src, uint8_t offset) {
  if(const Symbol* dst_symbol = dst.value.try_as<Symbol>()) {
    uint8_t outer;
    if(const uint8_t* local = func.local_table.find(*dst_symbol)) {
      func.push(dst, ScriptInstruction {CopyLocal, *local, src});
    } else if(find_outer(func, *dst_symbol, outer)) {
      func.push(dst, ScriptInstruction {StoreOuter, outer, src});
    } else {
      func.push(dst, ScriptInstruction {StoreGlobal, src});
      func.bytecode.back().bcu16 = _script->global(*dst_symbol);
    }
  } else if(dst.type == LSParser::NodeType::Access) {
    compile_access_chain(func, dst.children, offset, false);
    func.push(dst, ScriptInstruction {SetItem, uint8_t(offset + 1), offset, src});
  } else {
    error("Trying to set a literal value or something?");
  }
}
void LSCompiler::compile_operator(Function& func, const L::Array<LSParser::Node>& array, uint8_t offset, ScriptOpCode opcode) {
  L_ASSERT(array.size() > 1);
  compile(func, array[1], offset);
  for(uint32_t i(2); i < array.size(); i++) {
    compile(func, array[i], offset + 1);
    func.push(array[i], ScriptInstruction {opcode, offset, uint8_t(offset + 1)});
  }
}
void LSCompiler::compile_op_assign(Function& func, const Array<LSParser::Node>& array, uint8_t offset, ScriptOpCode opcode) {
  L_ASSERT(array.size() >= 3);
  // There's a special case for local targets but otherwise we'll do a copy, operate on it, then assign it back
  // To be honest this special case could be dealt with during optimization later
  if(array[1].value.is<Symbol>()) {
    if(uint8_t* found = func.local_table.find(array[1].value.as<Symbol>())) {
      for(uintptr_t i(2); i < array.size(); i++) {
        compile(func, array[i], offset);
        func.push(array[i], ScriptInstruction {opcode, *found, offset});
      }
      return;
    }
  }
  compile(func, array[1], offset);
  for(uintptr_t i(2); i < array.size(); i++) {
    compile(func, array[i], offset + 1);
    func.push(array[i], ScriptInstruction {opcode, offset, uint8_t(offset + 1)});
  }
  compile_assignment(func, array[1], offset, offset + 1);
}
void LSCompiler::compile_comparison(Function& func, const L::Array<LSParser::Node>& array, uint8_t offset, ScriptOpCode cmp, bool not) {
  L_ASSERT(array.size() >= 3);
  if(array.size() >= 3) {
    Array<uintptr_t> end_jumps; // Will jump *after* the comparison
    compile(func, array[1], offset + 1);
    for(uintptr_t i(2); i < array.size(); i++) {
      compile(func, array[i], uint8_t(offset + i));
      func.push(array[i], ScriptInstruction {cmp, offset, uint8_t(offset + i - 1), uint8_t(offset + i)});
      if(not) {
        func.push(array[i], ScriptInstruction {Not, offset});
      }
      func.push(array[i], ScriptInstruction {CondNotJump, offset});
      end_jumps.push(func.bytecode.size() - 1);
    }

    // Update all end jumps to jump here
    for(uintptr_t end_jump : end_jumps) {
      func.bytecode[end_jump].bc16 = int16_t(func.bytecode.size() - end_jump) - 1;
    }
  } else {
    error("Comparisons need at least two operands");
  }
}