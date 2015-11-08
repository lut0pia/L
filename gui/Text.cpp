#include "Text.h"

using namespace L;
using namespace GUI;

void GUI::Text::redraw() {
  Bitmap bmp(font.render(text));
  bmp.filter(color);
  bitmap(bmp);
}
GUI::Text::Text(const String& text, const String& font, Color color)
  : Image(), text(text), font(Font::get(font)), color(color) {
  redraw();
}
void GUI::Text::sText(const String& text, Color color) {
  this->text = text;
  this->color = color;
  redraw();
}


