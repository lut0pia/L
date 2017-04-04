#include "Resource.h"

#include "../image/Bitmap.h"
#include "../containers/Table.h"

using namespace L;

Table<uint32_t, Ref<GL::Texture> > _textures;
Table<uint32_t, Ref<GL::Mesh> > _meshes;
Table<uint32_t, Ref<Script::CodeFunction>> _scripts;

const Ref<GL::Texture>& Resource::texture(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _textures.find(h)) return *found;
  else return _textures[h] = ref<GL::Texture>(Bitmap(fp));
}
const Ref<GL::Mesh>& Resource::mesh(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _meshes.find(h)) return *found;
  else return _meshes[h] = ref<GL::Mesh>(fp);
}
const Ref<Script::CodeFunction>& Resource::script(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _scripts.find(h)) return *found;
  else {
    FileStream stream(fp, "rb");
    return _scripts[h] = ref<Script::CodeFunction>(Script::Context::read(stream));
  }
}
