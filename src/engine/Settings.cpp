#include "Settings.h"

#include "../container/Table.h"
#include "../stream/CFileStream.h"

using namespace L;

struct Setting {
  int _int;
  float _float;
  Symbol _symbol;
};

Table<Symbol, Setting> settings;

void Settings::load_file(const char* path) {
  CFileStream fs(path, "r");
  if(fs) {
    char line[4096];
    while(true) {
      // Read line
      fs.line(line, sizeof(line));
      if(!line[0] && fs.end()) break; // End of file

      // Find value
      char* value(line);
      while(*value && *value!='=')
        value++;
      if(!*value)
        L_ERRORF("Invalid .ini line:\n%s\nin %s", line, path);
      *value++ = '\0';
      settings[line] = Setting{
        atoi(value),
        float(atof(value)),
        value
      };
    }
  }
}
int Settings::get_int(const Symbol& name, int default_value) {
  if(auto found = settings.find(name))
    return found->_int;
  else return default_value;
}
float Settings::get_float(const Symbol& name, float default_value) {
  if(auto found = settings.find(name))
    return found->_float;
  else return default_value;
}
const Symbol& Settings::get_symbol(const Symbol& name, const Symbol& default_value) {
  if(auto found = settings.find(name))
    return found->_symbol;
  else return default_value;
}
