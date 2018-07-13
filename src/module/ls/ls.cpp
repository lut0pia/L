#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.h>
#include <L/src/script/Compiler.h>

using namespace L;

void ls_loader(Resource<Script::CodeFunction>::Slot& slot) {
  Buffer buffer(slot.read_source_file());
  Script::Compiler compiler;
  compiler.read((const char*)buffer.data(), buffer.size(), true);
  slot.value = ref<Script::CodeFunction>(compiler.function());
}

void ls_module_init() {
  Resource<Script::CodeFunction>::add_loader("ls", ls_loader);
}
