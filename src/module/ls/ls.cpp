#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/script/Compiler.h>

using namespace L;

void ls_loader(ResourceSlot& slot, Script::CodeFunction*& intermediate) {
  Buffer buffer(slot.read_source_file());
  Script::Compiler compiler;
  compiler.read((const char*)buffer.data(), buffer.size(), true);
  intermediate = Memory::new_type<Script::CodeFunction>(compiler.function());
}

void ls_module_init() {
  ResourceLoading<Script::CodeFunction>::add_loader("ls", ls_loader);
}
