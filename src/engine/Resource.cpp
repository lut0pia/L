#include "Resource.h"

#include "../font/PixelFont.h"
#include "../Interface.h"
#include "../image/Bitmap.h"
#include "../containers/Table.h"

using namespace L;

Table<uint32_t, Ref<GL::Texture> > _textures;
Table<uint32_t, Ref<GL::Mesh> > _meshes;
Table<uint32_t, Ref<Script::CodeFunction>> _scripts;
Table<uint32_t, Ref<Font>> _fonts;

void Resource::update() {
  for(auto&& font : _fonts)
    font.value()->updateTextMeshes();
}
const Ref<GL::Texture>& Resource::texture(const char* fp) {
  static Ref<GL::Texture> missing(ref<GL::Texture>(1,1,&Color::magenta));
  const uint32_t h(hash(fp));
  if(auto found = _textures.find(h)) return *found;
  else if(auto bmp = Interface<Bitmap>::fromFile(fp))
    return _textures[h] = ref<GL::Texture>(*bmp);
  else return missing;
}
const Ref<GL::Mesh>& Resource::mesh(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _meshes.find(h)) return *found;
  else return _meshes[h] = Interface<GL::Mesh>::fromFile(fp);
}
const Ref<Script::CodeFunction>& Resource::script(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _scripts.find(h)) return *found;
  else {
    CFileStream stream(fp, "rb");
    return _scripts[h] = ref<Script::CodeFunction>(Script::Context::read(stream));
  }
}
const Ref<Font>& Resource::font(const char* fp) {
  static const uint32_t defaultHash(hash("default"));
  static Ref<Font> pixel(ref<PixelFont>());
  const uint32_t h(hash(fp));
  if(h==defaultHash) return pixel;
  else if(auto found = _fonts.find(h)) return *found;
  else return _fonts[h] = Interface<Font>::fromFile(fp);
}
