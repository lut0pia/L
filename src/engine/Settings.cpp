#include "Settings.h"

#include "../container/Table.h"

using namespace L;

struct Setting {
  int _int;
  float _float;
  String _string;
};

static Table<Symbol, Setting*> settings;

static inline bool get_setting(const Symbol& name, Setting*& out_setting) {
  Setting*& setting = settings[name];
  out_setting = setting;
  if(setting==nullptr) {
    setting = Memory::new_type<Setting>();
    out_setting = setting;
    return true;
  }
  return false;
}

void Settings::set(const Symbol& name, const Var& value) {
  Setting* setting;
  get_setting(name, setting);
  setting->_int = value.get<int>();
  setting->_float = value.get<float>();
  setting->_string = value.get<String>();
}
const int& Settings::get_int(const Symbol& name, int default_value) {
  Setting* setting;
  if(get_setting(name, setting))
    set(name, default_value);
  return setting->_int;
}
const float& Settings::get_float(const Symbol& name, float default_value) {
  Setting* setting;
  if(get_setting(name, setting))
    set(name, default_value);
  return setting->_float;
}
const String& Settings::get_string(const Symbol& name, const String& default_value) {
  Setting* setting;
  if(get_setting(name, setting))
    set(name, default_value);
  return setting->_string;
}
