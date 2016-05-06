#include "Font.h"

#include "../containers/Ref.h"
#include "Pixel.h"
#include "../hash.h"

using namespace L;

Map<uint32_t,Ref<Font::Base> > fonts;

Font::Base& Font::get(const char* name) {
  static Font::Pixel pixel;
  return (fonts.has(fnv1a(name)))?*fonts[fnv1a(name)]:pixel;
}
void Font::set(Base* font, const char* name) {
  fonts[fnv1a(name)] = font;
}
