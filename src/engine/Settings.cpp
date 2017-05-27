#include "Settings.h"

#include "../containers/Table.h"
#include "../streams/CFileStream.h"

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
int Settings::get_int(const Symbol& name, int default) {
  if(auto found = settings.find(name))
    return found->_int;
  else return default;
}
float Settings::get_float(const Symbol& name, float default) {
  if(auto found = settings.find(name))
    return found->_float;
  else return default;
}
const Symbol& Settings::get_symbol(const Symbol& name, const Symbol& default) {
  if(auto found = settings.find(name))
    return found->_symbol;
  else return default;
}
