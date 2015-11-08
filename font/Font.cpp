#include "Font.h"

#include "../containers/Ref.h"
#include "Pixel.h"

using namespace L;

Map<String,Ref<Font::Base> > fonts;

Font::Base& Font::get(const String& name) {
  static Font::Pixel pixel;
  return (fonts.has(name))?*fonts[name]:pixel;
}
void Font::set(Base* font, const String& name) {
  fonts[name] = font;
}
