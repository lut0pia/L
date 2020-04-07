#include "script.h"

using namespace L;

uint16_t Script::constant(const Var& v) {
  for(uint16_t i(0); i < constants.size(); i++) {
    if(constants[i] == v) {
      return i;
    }
  }
  L_ASSERT(constants.size() < UINT16_MAX);
  constants.push(v);
  return uint8_t(constants.size() - 1);
}
uint16_t Script::global(Symbol sym) {
  for(uint8_t i(0); i < globals.size(); i++) {
    if(globals[i].name() == sym) {
      return i;
    }
  }
  L_ASSERT(globals.size() < UINT16_MAX);
  globals.push(sym);
  return uint8_t(globals.size() - 1);
}
void Script::print(Stream& s) {
  uintptr_t addr = 0;
#if !L_RLS
  uintptr_t line = 0;
#endif
  for(const ScriptInstruction& i : bytecode) {
#if !L_RLS
    if(addr < bytecode_line.size() && line != bytecode_line[addr]) {
      line = bytecode_line[addr];
      s << '\t' << source_lines[line - 1] << '\n';
    }
#endif
    s << ntos<10>(addr, 4) << ' ';
    switch(i.opcode) {
      case CopyLocal:    s << "CopyLocal:    " << i.a << " := " << i.bc8.b << "\n"; break;
      case LoadConst:    s << "LoadConst:    " << i.a << " := " << constants[i.bcu16] << "\n"; break;
      case LoadBool:     s << "LoadBool:     " << i.a << " := " << (i.bc8.b != 0) << "\n"; break;
      case LoadInt:      s << "LoadInt:      " << i.a << " := " << i.bc16 << "\n"; break;
      case LoadGlobal:   s << "LoadGlobal:   " << i.a << " := " << globals[i.bcu16].name() << "\n"; break;
      case StoreGlobal:  s << "StoreGlobal:  " << globals[i.bcu16].name() << " := " << i.a << "\n"; break;
      case LoadFun:      s << "LoadFun:      " << i.a << " := " << i.bc16 << "\n"; break;

      case LoadOuter:    s << "LoadOuter:    " << i.a << " := " << i.bc8.b << "\n"; break;
      case StoreOuter:   s << "StoreOuter:   " << i.a << " := " << i.bc8.b << "\n"; break;
      case CaptLocal:    s << "CaptLocal:    " << i.a << " := " << i.bc8.b << "\n"; break;
      case CaptOuter:    s << "CaptOuter:    " << i.a << " := " << i.bc8.b << "\n"; break;

      case MakeObject:   s << "MakeObject:   " << i.a << " := {}\n"; break;
      case MakeArray:    s << "MakeArray:    " << i.a << " := []\n"; break;
      case GetItem:      s << "GetItem:      " << i.bc8.c << " := " << i.a << "[" << i.bc8.b << "]\n"; break;
      case SetItem:      s << "SetItem:      " << i.a << "[" << i.bc8.b << "] := " << i.bc8.c << "\n"; break;
      case PushItem:     s << "PushItem:     " << i.a << "[] := " << i.bc8.b << "\n"; break;

      case MakeIterator: s << "MakeIterator: " << i.a << " := it(" << i.bc8.b << ")\n"; break;
      case Iterate:      s << "Iterate:      (" << i.a << "," << i.bc8.b << ") := iter(" << i.bc8.c << ")\n"; break;
      case IterEndJump:  s << "IterEndJump:  iter_end(" << i.a << ") => " << i.bc16 << "\n"; break;

      case Jump:         s << "Jump:         " << i.bc16 << "\n"; break;
      case CondJump:     s << "CondJump:     " << i.a << " => " << i.bc16 << "\n"; break;
      case CondNotJump:  s << "CondNotJump:  !" << i.a << " => " << i.bc16 << "\n"; break;

      case Add:          s << "Add:          " << i.a << " += " << i.bc8.b << "\n"; break;
      case Sub:          s << "Sub:          " << i.a << " -= " << i.bc8.b << "\n"; break;
      case Mul:          s << "Mul:          " << i.a << " *= " << i.bc8.b << "\n"; break;
      case Div:          s << "Div:          " << i.a << " /= " << i.bc8.b << "\n"; break;
      case Mod:          s << "Mod:          " << i.a << " %= " << i.bc8.b << "\n"; break;
      case Inv:          s << "Inv:          " << i.a << " := inv(" << i.a << ")\n"; break;
      case Not:          s << "Not:          " << i.a << " := !" << i.a << "\n"; break;

      case LessThan:     s << "LessThan:     " << i.a << " := " << i.bc8.b << " < " << i.bc8.c << "\n"; break;
      case LessEqual:    s << "LessEqual:    " << i.a << " := " << i.bc8.b << " <= " << i.bc8.c << "\n"; break;
      case Equal:        s << "Equal:        " << i.a << " := " << i.bc8.b << " == " << i.bc8.c << "\n"; break;

      case Call:         s << "Call:         " << i.a << " (" << i.bc8.b << " parameters)\n"; break;
      case Return:       s << "Return\n"; break;
      default: error("Unknown script instruction to print"); break;
    }
    addr++;
  }
}
