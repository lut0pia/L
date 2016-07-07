#include "Text.h"

#include "../font/Font.h"

using namespace L;
using namespace GUI;

void GUI::Text::redraw() {
  Bitmap bmp(_font.render(_text));
  bmp.filter(_color);
  bitmap(bmp);
}
GUI::Text::Text(const String& text, const String& font, Color color)
  : Image(), _text(text), _font(Font::get(font)), _color(color) {
  redraw();
}
void GUI::Text::text(const String& text, Color color) {
  if(_text!=text || _color!=color) {
    _text = text;
    _color = color;
    redraw();
  }
}


