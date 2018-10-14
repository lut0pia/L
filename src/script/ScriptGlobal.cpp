#include "ScriptGlobal.h"

#include "../container/Pool.h"
#include "../container/Table.h"

using namespace L;

static Pool<ScriptGlobal::Slot> slots;
static Table<Symbol, ScriptGlobal::Slot*> slots_by_name;

ScriptGlobal::ScriptGlobal(const Symbol& name) {
  if(Slot** found = slots_by_name.find(name)) {
    _slot = *found;
  } else {
    slots_by_name[name] = _slot = slots.construct();
    _slot->name = name;
  }
}
ScriptGlobal::ScriptGlobal(const Symbol& name, const Var& v) : ScriptGlobal(name) {
  value() = v;
}
Stream& L::operator<=(Stream& s, const ScriptGlobal& v) {
  return s <= v._slot->name;
}
Stream& L::operator>=(Stream& s, ScriptGlobal& v) {
  Symbol name;
  s >= name;
  v = name;
  return s;
}
