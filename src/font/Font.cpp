#include "Font.h"

#include "Pixel.h"
#include "../hash.h"

using namespace L;

Table<uint32_t, Ref<Font::Base>> fonts;

Font::Base& Font::get(const char* name) {
  static Ref<Font::Base> pixel(ref<Pixel>());
  const uint32_t h(fnv1a(name));
  if(auto font = fonts.find(h))
    return **font;
  else return *(fonts[h] = pixel);
}
void Font::set(const Ref<Base>& font, const char* name) {
  fonts[fnv1a(name)] = font;
}
void Font::update() {
  for(auto& font : fonts)
    font.value()->updateTextMeshes();
}
