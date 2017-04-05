#include "Resource.h"

#include "../font/Pixel.h"
#include "../Interface.h"
#include "../image/Bitmap.h"
#include "../containers/Table.h"

using namespace L;

Table<uint32_t, Ref<GL::Texture> > _textures;
Table<uint32_t, Ref<GL::Mesh> > _meshes;
Table<uint32_t, Ref<Script::CodeFunction>> _scripts;
Table<uint32_t, Ref<Font::Base>> _fonts;

void Resource::update() {
  for(auto&& font : _fonts)
    font.value()->updateTextMeshes();
}
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
const Ref<Font::Base>& Resource::font(const char* fp) {
  static const uint32_t defaultHash(hash("default"));
  static Ref<Font::Base> pixel(ref<Font::Pixel>());
  const uint32_t h(hash(fp));
  if(h==defaultHash) return pixel;
  else if(auto found = _fonts.find(h)) return *found;
  else {
    Ref<Font::Base> font;
    Interface<Ref<Font::Base>>::fromFile(font, fp);
    L_ASSERT(!font.null());
    return _fonts[h] = font;
  }
}
